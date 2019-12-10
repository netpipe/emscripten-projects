// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_DRIVER_H_INCLUDED__
#define __C_OGLES2_DRIVER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"

#ifdef _IRR_COMPILE_WITH_OGLES2_

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "EDriverFeatures.h"
#include "fast_atof.h"
#include "COGLES2ExtensionHandler.h"
#include "IContextManager.h"

#if defined(_IRR_WINDOWS_API_)
// include windows headers for HWND
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
#include "iOS/CIrrDeviceiOS.h"
#endif

#ifdef _MSC_VER
#pragma comment(lib, "libGLESv2.lib")
#endif

namespace irr
{
namespace video
{
	class COGLES2CallBridge;
	class COGLES2Texture;
	class COGLES2FixedPipelineRenderer;
	class COGLES2Renderer2D;
	class COGLES2NormalMapRenderer;
	class COGLES2ParallaxMapRenderer;

	class COGLES2Driver : public CNullDriver, public IMaterialRendererServices, public COGLES2ExtensionHandler
	{
		friend class COGLES2CallBridge;
		friend class COGLES2Texture;

	public:
		//! constructor
		COGLES2Driver(const SIrrlichtCreationParameters& params,
				io::IFileSystem* io
#if defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_WINDOWS_API_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_) || defined(_IRR_COMPILE_WITH_FB_DEVICE_)
		, IContextManager* contextManager
#elif defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		, CIrrDeviceIPhone* device
#endif
		);

		//! destructor
		virtual ~COGLES2Driver();

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
					SColor color=SColor(255, 0, 0, 0),
					const SExposedVideoData& videoData=SExposedVideoData(),
					core::rect<s32>* sourceRect=0);

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene();

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

		struct SHWBufferLink_opengl : public SHWBufferLink
		{
			SHWBufferLink_opengl(const scene::IMeshBuffer *meshBuffer)
			: SHWBufferLink(meshBuffer), vbo_verticesID(0), vbo_indicesID(0)
			, vbo_verticesSize(0), vbo_indicesSize(0)
			{}

			u32 vbo_verticesID; //tmp
			u32 vbo_indicesID; //tmp

			u32 vbo_verticesSize; //tmp
			u32 vbo_indicesSize; //tmp
		};

		bool updateVertexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);
		bool updateIndexHardwareBuffer(SHWBufferLink_opengl *HWBuffer);

		//! updates hardware buffer if needed
		virtual bool updateHardwareBuffer(SHWBufferLink *HWBuffer);

		//! Create hardware buffer from mesh
		virtual SHWBufferLink *createHardwareBuffer(const scene::IMeshBuffer* mb);

		//! Delete hardware buffer (only some drivers can)
		virtual void deleteHardwareBuffer(SHWBufferLink *HWBuffer);

		//! Draw hardware buffer
		virtual void drawHardwareBuffer(SHWBufferLink *HWBuffer);

		//! draws a vertex primitive list
		virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
				const void* indexList, u32 primitiveCount,
				E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType);

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
		{
			return FeatureEnabled[feature] && COGLES2ExtensionHandler::queryFeature(feature);
		}

		//! Sets a material.
		virtual void setMaterial(const SMaterial& material);

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(const video::ITexture* texture,
				const core::position2d<s32>& destPos,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

		//! draws a set of 2d images
		virtual void draw2DImageBatch(const video::ITexture* texture,
				const core::position2d<s32>& pos,
				const core::array<core::rect<s32> >& sourceRects,
				const core::array<s32>& indices, s32 kerningWidth = 0,
				const core::rect<s32>* clipRect = 0,
				SColor color = SColor(255, 255, 255, 255),
				bool useAlphaChannelOfTexture = false);

		//! Draws a part of the texture into the rectangle.
		virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
				const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
				const video::SColor* const colors = 0, bool useAlphaChannelOfTexture = false);

		void draw2DImageBatch(const video::ITexture* texture,
				const core::array<core::position2d<s32> >& positions,
				const core::array<core::rect<s32> >& sourceRects,
				const core::rect<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture);

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos,
				const core::rect<s32>* clip = 0);

		//!Draws an 2d rectangle with a gradient.
		virtual void draw2DRectangle(const core::rect<s32>& pos,
				SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
				const core::rect<s32>* clip = 0);

		//! Draws a 2d line.
		virtual void draw2DLine(const core::position2d<s32>& start,
				const core::position2d<s32>& end,
				SColor color = SColor(255, 255, 255, 255));

		//! Draws a single pixel
		virtual void drawPixel(u32 x, u32 y, const SColor & color);

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
				const core::vector3df& end,
				SColor color = SColor(255, 255, 255, 255));

		//! Draws a pixel
//			virtual void drawPixel(u32 x, u32 y, const SColor & color);

		//! Returns the name of the video driver.
		virtual const wchar_t* getName() const;

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights();

		//! adds a dynamic light
		virtual s32 addDynamicLight(const SLight& light);

		//! Turns a dynamic light on or off
		/** \param lightIndex: the index returned by addDynamicLight
		\param turnOn: true to turn the light on, false to turn it off */
		virtual void turnLightOn(s32 lightIndex, bool turnOn);

		//! returns the maximal amount of dynamic lights the device can handle
		virtual u32 getMaximalDynamicLightAmount() const;

		//! Returns the maximum texture size supported.
		virtual core::dimension2du getMaxTextureSize() const;

		//! Draws a shadow volume into the stencil buffer.
		virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible=0) _IRR_OVERRIDE_;

		//! Fills the stencil shadow with color.
		virtual void drawStencilShadow(bool clearStencilBuffer=false,
			video::SColor leftUpEdge = video::SColor(0,0,0,0),
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0)) _IRR_OVERRIDE_;

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area);

		//! Only used internally by the engine
		virtual void OnResize(const core::dimension2d<u32>& size);

		//! Returns type of video driver
		virtual E_DRIVER_TYPE getDriverType() const;

		//! get color format of the current color buffer
		virtual ECOLOR_FORMAT getColorFormat() const;

		//! Returns the transformation set by setTransform
		virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;

		//! Can be called by an IMaterialRenderer to make its work easier.
		virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastmaterial, bool resetAllRenderstates);

		//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
		virtual void setTextureRenderStates(const SMaterial& material, bool resetAllRenderstates);

		//! Get a vertex shader constant index.
		virtual s32 getVertexShaderConstantID(const c8* name);

		//! Get a pixel shader constant index.
		virtual s32 getPixelShaderConstantID(const c8* name);

		//! Sets a vertex shader constant.
		virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

		//! Sets a pixel shader constant.
		virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

		//! Sets a constant for the vertex shader based on an index.
		virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count);

		//! Int interface for the above.
		virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count);

		//! Sets a constant for the pixel shader based on an index.
		virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count);

		//! Int interface for the above.
		virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count);

		//! sets the current Texture
		bool setActiveTexture(u32 stage, const video::ITexture* texture);

		//! check if active texture is not equal null.
		bool isActiveTexture(u32 stage);

		//! disables all textures beginning with fromStage.
		bool disableTextures(u32 fromStage = 0);

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addShaderMaterial(const c8* vertexShaderProgram, const c8* pixelShaderProgram,
				IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData);

		//! Adds a new material renderer to the VideoDriver
		virtual s32 addHighLevelShaderMaterial(
				const c8* vertexShaderProgram,
				const c8* vertexShaderEntryPointName = 0,
				E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
				const c8* pixelShaderProgram = 0,
				const c8* pixelShaderEntryPointName = 0,
				E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
				const c8* geometryShaderProgram = 0,
				const c8* geometryShaderEntryPointName = "main",
				E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
				scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
				scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
				u32 verticesOut = 0,
				IShaderConstantSetCallBack* callback = 0,
				E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
				s32 userData=0,
				E_GPU_SHADING_LANGUAGE shadingLang = EGSL_DEFAULT);

		//! Returns pointer to the IGPUProgrammingServices interface.
		virtual IGPUProgrammingServices* getGPUProgrammingServices();

		//! Returns a pointer to the IVideoDriver interface.
		virtual IVideoDriver* getVideoDriver();

		//! Returns the maximum amount of primitives
		virtual u32 getMaximalPrimitiveCount() const;

		virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
				const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

		virtual bool setRenderTarget(video::ITexture* texture, bool clearBackBuffer,
				bool clearZBuffer, SColor color);

		//! set or reset special render targets
//			virtual bool setRenderTarget(video::E_RENDER_TARGET target, bool clearTarget,
//					bool clearZBuffer, SColor color);

		//! Sets multiple render targets
//			virtual bool setRenderTarget(const core::array<video::IRenderTarget>& texture,
//					bool clearBackBuffer=true, bool clearZBuffer=true, SColor color=SColor(0,0,0,0));

		//! Clears the ZBuffer.
		virtual void clearZBuffer();

		//! Returns an image created from the last rendered frame.
		virtual IImage* createScreenShot(video::ECOLOR_FORMAT format=video::ECF_UNKNOWN, video::E_RENDER_TARGET target=video::ERT_FRAME_BUFFER);

		//! checks if an OpenGL error has happened and prints it
		bool testGLError();

		//! checks if an OGLES1 error has happened and prints it
		bool testEGLError();

		//! Set/unset a clipping plane.
		virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable = false);

		//! returns the current amount of user clip planes set.
		u32 getClipPlaneCount() const;

		//! returns the 0 indexed Plane
		const core::plane3df& getClipPlane(u32 index) const;

		//! Enable/disable a clipping plane.
		virtual void enableClipPlane(u32 index, bool enable);

		//! Returns the graphics card vendor name.
		virtual core::stringc getVendorInfo()
		{
			return VendorName;
		};

		ITexture* createDepthTexture(ITexture* texture, bool shared = true);
		void removeDepthTexture(ITexture* texture);

		void removeTexture(ITexture* texture);

		void deleteFramebuffers(s32 n, const u32 *framebuffers);
		void deleteRenderbuffers(s32 n, const u32 *renderbuffers);

		// returns the current size of the screen or rendertarget
		virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const;

		//! Convert E_BLEND_FACTOR to OpenGL equivalent
		GLenum getGLBlend(E_BLEND_FACTOR factor) const;

		//! Get ZBuffer bits.
		GLenum getZBufferBits() const;

		//! Get current material.
		const SMaterial& getCurrentMaterial() const;

		//! Get bridge calls.
		COGLES2CallBridge* getBridgeCalls() const;

	private:

		class STextureStageCache
		{
		public:
			STextureStageCache()
			{
				for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
					CurrentTexture[i] = 0;
			}

			~STextureStageCache()
			{
				clear();
			}

			void set(u32 stage, const ITexture* tex)
			{
				if (stage < MATERIAL_MAX_TEXTURES)
				{
					const ITexture* oldTexture = CurrentTexture[stage];

					if (tex)
						tex->grab();

					CurrentTexture[stage] = tex;

					if (oldTexture)
						oldTexture->drop();
				}
			}

			const ITexture* operator[](int stage) const
			{
				if ((u32)stage < MATERIAL_MAX_TEXTURES)
					return CurrentTexture[stage];
				else
					return 0;
			}

			void remove(ITexture* tex)
			{
				for (s32 i = MATERIAL_MAX_TEXTURES-1; i>= 0; --i)
				{
					if (CurrentTexture[i] == tex)
					{
						tex->drop();
						CurrentTexture[i] = 0;
					}
				}
			}

			void clear()
			{
				for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
				{
					if (CurrentTexture[i])
					{
						CurrentTexture[i]->drop();
						CurrentTexture[i] = 0;
					}
				}
			}

		private:
			const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
		};

		void uploadClipPlane(u32 index);

		//! inits the opengl-es driver
		bool genericDriverInit(const core::dimension2d<u32>& screenSize, bool stencilBuffer);

		//! returns a device dependent texture from a software surface (IImage)
		virtual ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData);

		//! returns a device dependent texture from a software surface (IImage)
		virtual ITexture* createDeviceDependentTextureCube(const io::path& name, IImage* posXImage, IImage* negXImage,
			IImage* posYImage, IImage* negYImage, IImage* posZImage, IImage* negZImage);

		//! creates a transposed matrix in supplied GLfloat array to pass to OGLES1
		inline void createGLMatrix(float gl_matrix[16], const core::matrix4& m);

		inline void createGLTextureMatrix(float gl_matrix[16], const core::matrix4& m);

		//! Map Irrlicht wrap mode to OpenGL enum
		GLint getTextureWrapMode(u8 clamp) const;

		//! sets the needed renderstates
		void setRenderStates3DMode();

		//! sets the needed renderstates
		void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

		void createMaterialRenderers();

		void loadShaderData(const io::path& vertexShaderName, const io::path& fragmentShaderName, c8** vertexShaderData, c8** fragmentShaderData);

		core::stringw Name;
		core::matrix4 Matrices[ETS_COUNT];

		//! enumeration for rendering modes such as 2d and 3d for minimizing the switching of renderStates.
		enum E_RENDER_MODE
		{
			ERM_NONE = 0, // no render state has been set yet.
			ERM_2D, // 2d drawing rendermode
			ERM_3D // 3d rendering mode
		};

		E_RENDER_MODE CurrentRenderMode;
		//! bool to make all renderstates reset if set to true.
		bool ResetRenderStates;
		bool Transformation3DChanged;
		u8 AntiAlias;
		irr::io::path OGLES2ShaderPath;

		SMaterial Material, LastMaterial;
		COGLES2Texture* RenderTargetTexture;
		STextureStageCache CurrentTexture;
		core::array<ITexture*> DepthTextures;

		struct SUserClipPlane
		{
			core::plane3df Plane;
			bool Enabled;
		};

		core::array<SUserClipPlane> UserClipPlane;

		core::dimension2d<u32> CurrentRendertargetSize;

		core::stringc VendorName;

		core::matrix4 TextureFlipMatrix;

		//! Color buffer format
		ECOLOR_FORMAT ColorFormat;

		//! All the lights that have been requested; a hardware limited
		//! number of them will be used at once.
		struct RequestedLight
		{
			RequestedLight(SLight const & lightData)
					: LightData(lightData), DesireToBeOn(true) { }

			SLight LightData;
			bool DesireToBeOn;
		};

		core::array<RequestedLight> RequestedLights;

		COGLES2Renderer2D* MaterialRenderer2D;

		COGLES2CallBridge* BridgeCalls;

#if defined(_IRR_COMPILE_WITH_IPHONE_DEVICE_)
		CIrrDeviceIPhone* Device;
		GLuint ViewFramebuffer;
		GLuint ViewRenderbuffer;
		GLuint ViewDepthRenderbuffer;
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_WINDOWS_API_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_) || defined(_IRR_COMPILE_WITH_FB_DEVICE_)
		IContextManager* ContextManager;
#endif
	};

	//! This bridge between Irrlicht pseudo OpenGL calls
	//! and true OpenGL calls.

	class COGLES2CallBridge
	{
	public:
		COGLES2CallBridge(COGLES2Driver* driver);

		// Blending calls.

		void setBlendEquation(GLenum mode);

		void setBlendFunc(GLenum source, GLenum destination);

		void setBlendFuncSeparate(GLenum sourceRGB, GLenum destinationRGB, GLenum sourceAlpha, GLenum destinationAlpha);

		void setBlend(bool enable);

		// Color Mask.

		void setColorMask(bool red, bool green, bool blue, bool alpha);

		// Cull face calls.

		void setCullFaceFunc(GLenum mode);

		void setCullFace(bool enable);

		// Depth calls.

		void setDepthFunc(GLenum mode);

		void setDepthMask(bool enable);

		void setDepthTest(bool enable);

		// Program calls.

		void setProgram(GLuint program);

		// Texture calls.

		void resetTexture(const ITexture* texture);

		void setActiveTexture(GLenum texture);

		void getTexture(u32 stage, GLenum& type);

		void setTexture(u32 stage, GLenum type);

		// Viewport calls.

		void setViewport(const core::rect<s32>& viewport);

	private:
		COGLES2Driver* Driver;

		GLenum BlendEquation;
		GLenum BlendSourceRGB;
		GLenum BlendDestinationRGB;
		GLenum BlendSourceAlpha;
		GLenum BlendDestinationAlpha;
		bool Blend;

		bool ColorMask[4];

		GLenum CullFaceMode;
		bool CullFace;

		GLenum DepthFunc;
		bool DepthMask;
		bool DepthTest;

		GLuint Program;

		GLenum ActiveTexture;

		const ITexture* Texture[MATERIAL_MAX_TEXTURES];
		GLenum TextureType[MATERIAL_MAX_TEXTURES];

		core::rect<s32> Viewport;
	};

} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_

#endif
