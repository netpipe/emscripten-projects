/**
 * SDL_sound; An abstract sound format decoding API.
 *
 * Please see the file LICENSE.txt in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

/**
 * This file implements the core API, which is relatively simple.
 *   The real meat of SDL_sound is in the decoders.
 *
 * Documentation is in SDL_sound.h ... It's verbose, honest.  :)
 */

#define __SDL_SOUND_INTERNAL__
#include "SDL_sound_internal.h"

/* The various decoder drivers... */

/* All these externs may be missing; we check SOUND_SUPPORTS_xxx before use. */
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_MODPLUG;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_MP3;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_WAV;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_AIFF;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_AU;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_VORBIS;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_VOC;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_RAW;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_SHN;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_FLAC;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_QuickTime;
extern const Sound_DecoderFunctions __Sound_DecoderFunctions_CoreAudio;

typedef struct
{
    int available;
    const Sound_DecoderFunctions *funcs;
} decoder_element;

static decoder_element decoders[] =
{
#if SOUND_SUPPORTS_MODPLUG
    { 0, &__Sound_DecoderFunctions_MODPLUG },
#endif
#if SOUND_SUPPORTS_MP3
    { 0, &__Sound_DecoderFunctions_MP3 },
#endif
#if SOUND_SUPPORTS_WAV
    { 0, &__Sound_DecoderFunctions_WAV },
#endif
#if SOUND_SUPPORTS_AIFF
    { 0, &__Sound_DecoderFunctions_AIFF },
#endif
#if SOUND_SUPPORTS_AU
    { 0, &__Sound_DecoderFunctions_AU },
#endif
#if SOUND_SUPPORTS_VORBIS
    { 0, &__Sound_DecoderFunctions_VORBIS },
#endif
#if SOUND_SUPPORTS_VOC
    { 0, &__Sound_DecoderFunctions_VOC },
#endif
#if SOUND_SUPPORTS_RAW
    { 0, &__Sound_DecoderFunctions_RAW },
#endif
#if SOUND_SUPPORTS_SHN
    { 0, &__Sound_DecoderFunctions_SHN },
#endif
#if SOUND_SUPPORTS_FLAC
    { 0, &__Sound_DecoderFunctions_FLAC },
#endif
#if SOUND_SUPPORTS_COREAUDIO
    { 0, &__Sound_DecoderFunctions_CoreAudio },
#endif

    { 0, NULL }
};



/* General SDL_sound state ... */

typedef struct __SOUND_ERRMSGTYPE__
{
    Uint32 tid;
    int error_available;
    char error_string[128];
    struct __SOUND_ERRMSGTYPE__ *next;
} ErrMsg;

static ErrMsg *error_msgs = NULL;
static SDL_mutex *errorlist_mutex = NULL;

static Sound_Sample *sample_list = NULL;  /* this is a linked list. */
static SDL_mutex *samplelist_mutex = NULL;

static const Sound_DecoderInfo **available_decoders = NULL;
static int initialized = 0;


/* functions ... */

void Sound_GetLinkedVersion(Sound_Version *ver)
{
    if (ver != NULL)
    {
        ver->major = SOUND_VER_MAJOR;
        ver->minor = SOUND_VER_MINOR;
        ver->patch = SOUND_VER_PATCH;
    } /* if */
} /* Sound_GetLinkedVersion */


int Sound_Init(void)
{
    size_t i;
    size_t pos = 0;
    size_t total = sizeof (decoders) / sizeof (decoders[0]);
    BAIL_IF_MACRO(initialized, ERR_IS_INITIALIZED, 0);

    sample_list = NULL;
    error_msgs = NULL;

    available_decoders = (const Sound_DecoderInfo **)
                            SDL_calloc(total, sizeof (Sound_DecoderInfo *));
    BAIL_IF_MACRO(available_decoders == NULL, ERR_OUT_OF_MEMORY, 0);

    SDL_InitSubSystem(SDL_INIT_AUDIO);

    errorlist_mutex = SDL_CreateMutex();
    samplelist_mutex = SDL_CreateMutex();

    for (i = 0; decoders[i].funcs != NULL; i++)
    {
        decoders[i].available = decoders[i].funcs->init();
        if (decoders[i].available)
        {
            available_decoders[pos] = &(decoders[i].funcs->info);
            pos++;
        } /* if */
    } /* for */

    initialized = 1;
    return 1;
} /* Sound_Init */


int Sound_Quit(void)
{
    ErrMsg *err;
    ErrMsg *nexterr = NULL;
    size_t i;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);

    while (((volatile Sound_Sample *) sample_list) != NULL)
        Sound_FreeSample(sample_list);

    initialized = 0;

    SDL_DestroyMutex(samplelist_mutex);
    samplelist_mutex = NULL;
    sample_list = NULL;

    for (i = 0; decoders[i].funcs != NULL; i++)
    {
        if (decoders[i].available)
        {
            decoders[i].funcs->quit();
            decoders[i].available = 0;
        } /* if */
    } /* for */

    if (available_decoders != NULL)
        SDL_free((void *) available_decoders);
    available_decoders = NULL;

    /* clean up error state for each thread... */
    SDL_LockMutex(errorlist_mutex);
    for (err = error_msgs; err != NULL; err = nexterr)
    {
        nexterr = err->next;
        SDL_free(err);
    } /* for */
    error_msgs = NULL;
    SDL_UnlockMutex(errorlist_mutex);
    SDL_DestroyMutex(errorlist_mutex);
    errorlist_mutex = NULL;

    return 1;
} /* Sound_Quit */


const Sound_DecoderInfo **Sound_AvailableDecoders(void)
{
    return available_decoders;  /* READ. ONLY. */
} /* Sound_AvailableDecoders */


static ErrMsg *findErrorForCurrentThread(void)
{
    ErrMsg *i;
    Uint32 tid;

    if (error_msgs != NULL)
    {
        tid = SDL_ThreadID();

        SDL_LockMutex(errorlist_mutex);
        for (i = error_msgs; i != NULL; i = i->next)
        {
            if (i->tid == tid)
            {
                SDL_UnlockMutex(errorlist_mutex);
                return i;
            } /* if */
        } /* for */
        SDL_UnlockMutex(errorlist_mutex);
    } /* if */

    return NULL;   /* no error available. */
} /* findErrorForCurrentThread */


const char *Sound_GetError(void)
{
    const char *retval = NULL;
    ErrMsg *err;

    if (!initialized)
        return ERR_NOT_INITIALIZED;

    err = findErrorForCurrentThread();
    if ((err != NULL) && (err->error_available))
    {
        retval = err->error_string;
        err->error_available = 0;
    } /* if */

    return retval;
} /* Sound_GetError */


void Sound_ClearError(void)
{
    ErrMsg *err;

    if (!initialized)
        return;

    err = findErrorForCurrentThread();
    if (err != NULL)
        err->error_available = 0;
} /* Sound_ClearError */


/*
 * This is declared in the internal header.
 */
void __Sound_SetError(const char *str)
{
    ErrMsg *err;

    if (str == NULL)
        return;

    SNDDBG(("__Sound_SetError(\"%s\");%s\n", str,
              (initialized) ? "" : " [NOT INITIALIZED!]"));

    if (!initialized)
        return;

    err = findErrorForCurrentThread();
    if (err == NULL)
    {
        err = (ErrMsg *) SDL_calloc(1, sizeof (ErrMsg));
        if (err == NULL)
            return;   /* uhh...? */

        err->tid = SDL_ThreadID();

        SDL_LockMutex(errorlist_mutex);
        err->next = error_msgs;
        error_msgs = err;
        SDL_UnlockMutex(errorlist_mutex);
    } /* if */

    err->error_available = 1;
    SDL_strlcpy(err->error_string, str, sizeof (err->error_string));
} /* __Sound_SetError */


Uint32 __Sound_convertMsToBytePos(Sound_AudioInfo *info, Uint32 ms)
{
    /* "frames" == "sample frames" */
    float frames_per_ms = ((float) info->rate) / 1000.0f;
    Uint32 frame_offset = (Uint32) (frames_per_ms * ((float) ms));
    Uint32 frame_size = (Uint32) ((info->format & 0xFF) / 8) * info->channels;
    return frame_offset * frame_size;
} /* __Sound_convertMsToBytePos */


/*
 * Allocate a Sound_Sample, and fill in most of its fields. Those that need
 *  to be filled in later, by a decoder, will be initialized to zero.
 */
static Sound_Sample *alloc_sample(SDL_RWops *rw, Sound_AudioInfo *desired,
                                    Uint32 bufferSize)
{
    /*
     * !!! FIXME: We're going to need to pool samples, since the mixer
     * !!! FIXME:  might be allocating tons of these on a regular basis.
     */
    Sound_Sample *retval = SDL_calloc(1, sizeof (Sound_Sample));
    Sound_SampleInternal *internal = SDL_calloc(1, sizeof (Sound_SampleInternal));
    if ((retval == NULL) || (internal == NULL))
    {
        __Sound_SetError(ERR_OUT_OF_MEMORY);
        if (retval)
            SDL_free(retval);
        if (internal)
            SDL_free(internal);

        return NULL;
    } /* if */

    SDL_assert(bufferSize > 0);
    retval->buffer = SDL_calloc(1, bufferSize);  /* pure ugly. */
    if (!retval->buffer)
    {
        __Sound_SetError(ERR_OUT_OF_MEMORY);
        SDL_free(internal);
        SDL_free(retval);
        return NULL;
    } /* if */
    retval->buffer_size = bufferSize;

    if (desired != NULL)
        SDL_memcpy(&retval->desired, desired, sizeof (Sound_AudioInfo));

    internal->rw = rw;
    retval->opaque = internal;
    return retval;
} /* alloc_sample */


#if (defined DEBUG_CHATTER)
static  const char *fmt_to_str(Uint16 fmt)
{
    switch(fmt)
    {
        case AUDIO_U8:
            return "U8";
        case AUDIO_S8:
            return "S8";
        case AUDIO_U16LSB:
            return "U16LSB";
        case AUDIO_S16LSB:
            return "S16LSB";
        case AUDIO_S32LSB:
            return "S32LSB";
        case AUDIO_F32LSB:
            return "F32LSB";
        case AUDIO_U16MSB:
            return "U16MSB";
        case AUDIO_S16MSB:
            return "S16MSB";
        case AUDIO_S32MSB:
            return "S32MSB";
        case AUDIO_F32MSB:
            return "F32MSB";
    } /* switch */

    return "Unknown";
} /* fmt_to_str */
#endif


/*
 * The bulk of the Sound_NewSample() work is done here...
 *  Ask the specified decoder to handle the data in (rw), and if
 *  so, construct the Sound_Sample. Otherwise, try to wind (rw)'s stream
 *  back to where it was, and return false.
 */
static int init_sample(const Sound_DecoderFunctions *funcs,
                        Sound_Sample *sample, const char *ext,
                        Sound_AudioInfo *_desired)
{
    Sound_SampleInternal *internal = (Sound_SampleInternal *) sample->opaque;
    Sound_AudioInfo desired;
    int pos = SDL_RWtell(internal->rw);

        /* fill in the funcs for this decoder... */
    sample->decoder = &funcs->info;
    internal->funcs = funcs;
    if (!funcs->open(sample, ext))
    {
        SDL_RWseek(internal->rw, pos, SEEK_SET);  /* set for next try... */
        return 0;
    } /* if */

    /* success; we've got a decoder! */

    /* Now we need to set up the conversion buffer... */

    if (_desired == NULL)
        SDL_memcpy(&desired, &sample->actual, sizeof (Sound_AudioInfo));
    else
    {
        desired.format = _desired->format ? _desired->format : sample->actual.format;
        desired.channels = _desired->channels ? _desired->channels : sample->actual.channels;
        desired.rate = _desired->rate ? _desired->rate : sample->actual.rate;
    } /* else */

    if (SDL_BuildAudioCVT(&internal->sdlcvt,
                            sample->actual.format,
                            sample->actual.channels,
                            sample->actual.rate,
                            desired.format,
                            desired.channels,
                            desired.rate) == -1)
    {
        __Sound_SetError(SDL_GetError());
        funcs->close(sample);
        SDL_RWseek(internal->rw, pos, SEEK_SET);  /* set for next try... */
        return 0;
    } /* if */

    if (internal->sdlcvt.len_mult > 1)
    {
        void *rc = SDL_realloc(sample->buffer, sample->buffer_size * internal->sdlcvt.len_mult);
        if (rc == NULL)
        {
            funcs->close(sample);
            SDL_RWseek(internal->rw, pos, SEEK_SET);  /* set for next try... */
            return 0;
        } /* if */

        sample->buffer = rc;
    } /* if */

        /* these pointers are all one and the same. */
    SDL_memcpy(&sample->desired, &desired, sizeof (Sound_AudioInfo));
    internal->sdlcvt.buf = internal->buffer = sample->buffer;
    internal->buffer_size = sample->buffer_size / internal->sdlcvt.len_mult;
    internal->sdlcvt.len = internal->buffer_size;

    /* Prepend our new Sound_Sample to the sample_list... */
    SDL_LockMutex(samplelist_mutex);
    internal->next = sample_list;
    if (sample_list != NULL)
        ((Sound_SampleInternal *) sample_list->opaque)->prev = sample;
    sample_list = sample;
    SDL_UnlockMutex(samplelist_mutex);

    SNDDBG(("New sample DESIRED format: %s format, %d rate, %d channels.\n",
            fmt_to_str(sample->desired.format),
            sample->desired.rate,
            sample->desired.channels));

    SNDDBG(("New sample ACTUAL format: %s format, %d rate, %d channels.\n",
            fmt_to_str(sample->actual.format),
            sample->actual.rate,
            sample->actual.channels));

    SNDDBG(("On-the-fly conversion: %s.\n",
            internal->sdlcvt.needed ? "ENABLED" : "DISABLED"));

    return 1;
} /* init_sample */


Sound_Sample *Sound_NewSample(SDL_RWops *rw, const char *ext,
                              Sound_AudioInfo *desired, Uint32 bSize)
{
    Sound_Sample *retval;
    decoder_element *decoder;

    /* sanity checks. */
    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, NULL);
    BAIL_IF_MACRO(rw == NULL, ERR_INVALID_ARGUMENT, NULL);

    retval = alloc_sample(rw, desired, bSize);
    if (!retval)
        return NULL;  /* alloc_sample() sets error message... */

    if (ext != NULL)
    {
        for (decoder = &decoders[0]; decoder->funcs != NULL; decoder++)
        {
            if (decoder->available)
            {
                const char **decoderExt = decoder->funcs->info.extensions;
                while (*decoderExt)
                {
                    if (SDL_strcasecmp(*decoderExt, ext) == 0)
                    {
                        if (init_sample(decoder->funcs, retval, ext, desired))
                            return retval;
                        break;  /* done with this decoder either way. */
                    } /* if */
                    decoderExt++;
                } /* while */
            } /* if */
        } /* for */
    } /* if */

    /* no direct extension match? Try everything we've got... */
    for (decoder = &decoders[0]; decoder->funcs != NULL; decoder++)
    {
        if (decoder->available)
        {
            int should_try = 1;
            const char **decoderExt = decoder->funcs->info.extensions;

                /* skip if we would have tried decoder above... */
            while (*decoderExt)
            {
                if (SDL_strcasecmp(*decoderExt, ext) == 0)
                {
                    should_try = 0;
                    break;
                } /* if */
                decoderExt++;
            } /* while */

            if (should_try)
            {
                if (init_sample(decoder->funcs, retval, ext, desired))
                    return retval;
            } /* if */
        } /* if */
    } /* for */

    /* nothing could handle the sound data... */
    SDL_free(retval->opaque);
    if (retval->buffer != NULL)
        SDL_free(retval->buffer);
    SDL_free(retval);
    SDL_RWclose(rw);
    __Sound_SetError(ERR_UNSUPPORTED_FORMAT);
    return NULL;
} /* Sound_NewSample */


Sound_Sample *Sound_NewSampleFromFile(const char *filename,
                                      Sound_AudioInfo *desired,
                                      Uint32 bufferSize)
{
    const char *ext;
    SDL_RWops *rw;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, NULL);
    BAIL_IF_MACRO(filename == NULL, ERR_INVALID_ARGUMENT, NULL);

    ext = SDL_strrchr(filename, '.');
    rw = SDL_RWFromFile(filename, "rb");
    BAIL_IF_MACRO(rw == NULL, SDL_GetError(), NULL);

    if (ext != NULL)
        ext++;

    return Sound_NewSample(rw, ext, desired, bufferSize);
} /* Sound_NewSampleFromFile */


Sound_Sample *Sound_NewSampleFromMem(const Uint8 *data,
                                     Uint32 size,
                                     const char *ext,
                                     Sound_AudioInfo *desired,
                                     Uint32 bufferSize)
{
    SDL_RWops *rw;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, NULL);
    BAIL_IF_MACRO(data == NULL, ERR_INVALID_ARGUMENT, NULL);
    BAIL_IF_MACRO(size == 0, ERR_INVALID_ARGUMENT, NULL);

    rw = SDL_RWFromConstMem(data, size);
    BAIL_IF_MACRO(rw == NULL, SDL_GetError(), NULL);

    return Sound_NewSample(rw, ext, desired, bufferSize);
} /* Sound_NewSampleFromMem */


void Sound_FreeSample(Sound_Sample *sample)
{
    Sound_SampleInternal *internal;

    if (!initialized)
    {
        __Sound_SetError(ERR_NOT_INITIALIZED);
        return;
    } /* if */

    if (sample == NULL)
    {
        __Sound_SetError(ERR_INVALID_ARGUMENT);
        return;
    } /* if */

    internal = (Sound_SampleInternal *) sample->opaque;

    SDL_LockMutex(samplelist_mutex);

    /* update the sample_list... */
    if (internal->prev != NULL)
    {
        Sound_SampleInternal *prevInternal;
        prevInternal = (Sound_SampleInternal *) internal->prev->opaque;
        prevInternal->next = internal->next;
    } /* if */
    else
    {
        SDL_assert(sample_list == sample);
        sample_list = internal->next;
    } /* else */

    if (internal->next != NULL)
    {
        Sound_SampleInternal *nextInternal;
        nextInternal = (Sound_SampleInternal *) internal->next->opaque;
        nextInternal->prev = internal->prev;
    } /* if */

    SDL_UnlockMutex(samplelist_mutex);

    /* nuke it... */
    internal->funcs->close(sample);

    if (internal->rw != NULL)  /* this condition is a "just in case" thing. */
        SDL_RWclose(internal->rw);

    if ((internal->buffer != NULL) && (internal->buffer != sample->buffer))
        SDL_free(internal->buffer);

    SDL_free(internal);

    if (sample->buffer != NULL)
        SDL_free(sample->buffer);

    SDL_free(sample);
} /* Sound_FreeSample */


int Sound_SetBufferSize(Sound_Sample *sample, Uint32 newSize)
{
    void *newBuf = NULL;
    Sound_SampleInternal *internal = NULL;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);
    BAIL_IF_MACRO(sample == NULL, ERR_INVALID_ARGUMENT, 0);
    internal = ((Sound_SampleInternal *) sample->opaque);
    newBuf = SDL_realloc(sample->buffer, newSize * internal->sdlcvt.len_mult);
    BAIL_IF_MACRO(newBuf == NULL, ERR_OUT_OF_MEMORY, 0);

    internal->sdlcvt.buf = internal->buffer = sample->buffer = newBuf;
    sample->buffer_size = newSize;
    internal->buffer_size = newSize / internal->sdlcvt.len_mult;
    internal->sdlcvt.len = internal->buffer_size;

    return 1;
} /* Sound_SetBufferSize */


Uint32 Sound_Decode(Sound_Sample *sample)
{
    Sound_SampleInternal *internal = NULL;
    Uint32 retval = 0;

        /* a boatload of sanity checks... */
    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);
    BAIL_IF_MACRO(sample == NULL, ERR_INVALID_ARGUMENT, 0);
    BAIL_IF_MACRO(sample->flags & SOUND_SAMPLEFLAG_ERROR, ERR_PREV_ERROR, 0);
    BAIL_IF_MACRO(sample->flags & SOUND_SAMPLEFLAG_EOF, ERR_PREV_EOF, 0);

    internal = (Sound_SampleInternal *) sample->opaque;

    SDL_assert(sample->buffer != NULL);
    SDL_assert(sample->buffer_size > 0);
    SDL_assert(internal->buffer != NULL);
    SDL_assert(internal->buffer_size > 0);

        /* reset EAGAIN. Decoder can flip it back on if it needs to. */
    sample->flags &= ~SOUND_SAMPLEFLAG_EAGAIN;
    retval = internal->funcs->read(sample);

    if (retval > 0 && internal->sdlcvt.needed)
    {
        internal->sdlcvt.len = retval;
        SDL_ConvertAudio(&internal->sdlcvt);
        retval = internal->sdlcvt.len_cvt;
    } /* if */

    return retval;
} /* Sound_Decode */


Uint32 Sound_DecodeAll(Sound_Sample *sample)
{
    Sound_SampleInternal *internal = NULL;
    void *buf = NULL;
    Uint32 newBufSize = 0;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);
    BAIL_IF_MACRO(sample->flags & SOUND_SAMPLEFLAG_EOF, ERR_PREV_EOF, 0);
    BAIL_IF_MACRO(sample->flags & SOUND_SAMPLEFLAG_ERROR, ERR_PREV_ERROR, 0);

    internal = (Sound_SampleInternal *) sample->opaque;

    while ( ((sample->flags & SOUND_SAMPLEFLAG_EOF) == 0) &&
            ((sample->flags & SOUND_SAMPLEFLAG_ERROR) == 0) )
    {
        Uint32 br = Sound_Decode(sample);
        void *ptr = SDL_realloc(buf, newBufSize + br);
        if (ptr == NULL)
        {
            sample->flags |= SOUND_SAMPLEFLAG_ERROR;
            __Sound_SetError(ERR_OUT_OF_MEMORY);
        } /* if */
        else
        {
            buf = ptr;
            SDL_memcpy( ((char *) buf) + newBufSize, sample->buffer, br );
            newBufSize += br;
        } /* else */
    } /* while */

    if (buf == NULL)  /* ...in case first call to SDL_realloc() fails... */
        return sample->buffer_size;

    if (internal->buffer != sample->buffer)
        SDL_free(internal->buffer);

    SDL_free(sample->buffer);

    internal->sdlcvt.buf = internal->buffer = sample->buffer = buf;
    sample->buffer_size = newBufSize;
    internal->buffer_size = newBufSize / internal->sdlcvt.len_mult;
    internal->sdlcvt.len = internal->buffer_size;

    return newBufSize;
} /* Sound_DecodeAll */


int Sound_Rewind(Sound_Sample *sample)
{
    Sound_SampleInternal *internal;
    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);

    internal = (Sound_SampleInternal *) sample->opaque;
    if (!internal->funcs->rewind(sample))
    {
        sample->flags |= SOUND_SAMPLEFLAG_ERROR;
        return 0;
    } /* if */

    sample->flags &= ~SOUND_SAMPLEFLAG_EAGAIN;
    sample->flags &= ~SOUND_SAMPLEFLAG_ERROR;
    sample->flags &= ~SOUND_SAMPLEFLAG_EOF;

    return 1;
} /* Sound_Rewind */


int Sound_Seek(Sound_Sample *sample, Uint32 ms)
{
    Sound_SampleInternal *internal;

    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, 0);
    if (!(sample->flags & SOUND_SAMPLEFLAG_CANSEEK))
        BAIL_MACRO(ERR_CANNOT_SEEK, 0);

    internal = (Sound_SampleInternal *) sample->opaque;
    BAIL_IF_MACRO(!internal->funcs->seek(sample, ms), NULL, 0);

    sample->flags &= ~SOUND_SAMPLEFLAG_EAGAIN;
    sample->flags &= ~SOUND_SAMPLEFLAG_ERROR;
    sample->flags &= ~SOUND_SAMPLEFLAG_EOF;

    return 1;
} /* Sound_Rewind */


Sint32 Sound_GetDuration(Sound_Sample *sample)
{
    Sound_SampleInternal *internal;
    BAIL_IF_MACRO(!initialized, ERR_NOT_INITIALIZED, -1);
    internal = (Sound_SampleInternal *) sample->opaque;
    return internal->total_time;
} /* Sound_GetDuration */

/* end of SDL_sound.c ... */

