package net.qdating.player;

import net.qdating.LSConfig;
import net.qdating.LSConfig.FillMode;
import net.qdating.filter.LSImageFilter;
import net.qdating.filter.LSImageInputYuvFilter;
import net.qdating.filter.LSImageOutputFilter;
import net.qdating.utils.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.media.MediaCodecInfo;
import android.opengl.GLSurfaceView.Renderer;

/**
 * 视频输入裁剪
 * 魅族的GLES不支持GL_RGB
 * 
 * @author max
 */
public class LSVideoHardPlayerRenderer implements Renderer {
	/**
	 * 预览界面宽度
	 */
	int previewWidth = 0;
	/**
	 * 预览界面高度
	 */
	int previewHeight = 0;
	/**
	 * 原始视频宽度
	 */
	int originalWidth = 0;
	/**
	 * 原始视频高度
	 */
	int originalHeight = 0;
	/**
	 * 预览纹理Id
	 */
	private int[] glTextureId = null;
	/**
	 * 显示原始图片滤镜
	 */
	private LSImageInputYuvFilter inputYuvFilter = null;
	private LSImageOutputFilter outputFilter = null;

	private LSImageFilter customFilter = null;
	private boolean bCustomFilterChange = false;

	public LSVideoHardPlayerRenderer(FillMode fillMode) {
        inputYuvFilter = new LSImageInputYuvFilter();

		outputFilter = new LSImageOutputFilter();
		outputFilter.fillMode = fillMode;
	}

	/**
	 * 初始化
	 */
	public void init() {
		Log.d(LSConfig.TAG, String.format("LSVideoHardPlayerRenderer::init( this : 0x%x )", hashCode()));

		inputYuvFilter.setFilter(outputFilter);
	}

	/**
	 * 反初始化
	 */
	public void uninit() {
		Log.d(LSConfig.TAG, String.format("LSVideoHardPlayerRenderer::uninit( this : 0x%x )", hashCode()));
	}

	/**
	 * 设置自定义滤镜
	 * @param customFilter 自定义滤镜
	 */
	public void setCustomFilter(LSImageFilter customFilter) {
		synchronized (this) {
			if( this.customFilter != customFilter ) {
				this.customFilter = customFilter;
				bCustomFilterChange = true;

				if (this.customFilter != null) {
					inputYuvFilter.setFilter(this.customFilter);
					this.customFilter.setFilter(outputFilter);
				} else {
					inputYuvFilter.setFilter(outputFilter);
				}
			}
		}
	}

	/**
	 * 获取自定义滤镜
	 * @return 自定义滤镜
	 */
	public LSImageFilter getCustomFilter() {
		return this.customFilter;
	}

	public void updateDecodeFrame(LSVideoHardDecoderFrame videoFrame) {
		synchronized (this) {
			switch ( videoFrame.format ) {
				case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar: {
					inputYuvFilter.updateYuv420PFrame(videoFrame.byteBufferY, videoFrame.byteSizeY, videoFrame.byteBufferU, videoFrame.byteSizeU, videoFrame.byteBufferV, videoFrame.byteSizeV);
				}break;
				case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar: {
					inputYuvFilter.updateYuv420SPFrame(videoFrame.byteBufferY, videoFrame.byteSizeY, videoFrame.byteBufferUV, videoFrame.byteSizeUV);
				}break;
			}
		}
	}
	
	public void setOriginalSize(int width, int height) {
		Log.d(LSConfig.TAG,
				String.format("LSVideoHardPlayerRenderer::setOriginalSize( "
								+ "this : 0x%x, "
								+ "originalWidth : %d, "
								+ "originalHeight : %d "
								+ ")",
						hashCode(),
						width,
						height
				)
		);
		
		originalWidth = width;
		originalHeight = height;
	}
	
	@Override
	public void onDrawFrame(GL10 gl) {
		// TODO Auto-generated method stub
		
		// 重绘背景为黑色
		gl.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT );
		    
		synchronized (this) {
			if( bCustomFilterChange ) {
				if( customFilter != null ) {
					customFilter.init();
				}
				bCustomFilterChange = false;
			}

			inputYuvFilter.draw(glTextureId[0], originalWidth, originalHeight);
		}
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		// TODO Auto-generated method stub
        Log.i(LSConfig.TAG,
				String.format("LSVideoHardPlayerRenderer::onSurfaceChanged( "
								+ "this : 0x%x, "
								+ "width : %d, "
								+ "height : %d "
								+ ") ",
						hashCode(),
						width,
						height
				)
		);
        
        previewWidth = width;
        previewHeight = height;

		outputFilter.changeViewPointSize(previewWidth, previewHeight);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		// TODO Auto-generated method stub
        Log.d(LSConfig.TAG, String.format("LSVideoHardPlayerRenderer::onSurfaceCreated( this : 0x%x )", hashCode()));
		
		// 创建纹理
		glTextureId = LSImageFilter.genPixelTexture();
        
		inputYuvFilter.init();
		outputFilter.init();
	}

}
