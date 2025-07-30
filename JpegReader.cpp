#include "JpegReader.h"

#include <cwchar>
#include <locale>
#include <windows.h>

extern "C" {
#include "jpeglib.h"
}
#include <setjmp.h>     // 用于错误处理

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include <stb_image.h>

struct CustomErrorMsg
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmpBuffer;
};
typedef struct CustomErrorMsg* pErrorMgr;

// 自定义错误处理函数
void CustomErrorExit(j_common_ptr cinfo)
{
    pErrorMgr err = (pErrorMgr)cinfo->err;
    (*cinfo->err->output_message)(cinfo);   // 打印错误信息
    longjmp(err->setjmpBuffer, 1);          // 跳转回 setjmp 点
}

JpegReader::JpegReader() {}

Result JpegReader::Decode()
{
    // DecodeImage();
    return Result::SUCCESS;
}

Result JpegReader::DecodeImage(
    const std::string&                                  filepath,
    std::shared_ptr<ImageData>&                         imageData,
    std::vector<std::pair<std::string, std::string>>&   exif)
{
    // 读取文件EXIF信息
    Orientation orientation;
    ParseExif(filepath, exif, orientation);

    // 解码文件数据
    struct jpeg_decompress_struct cinfo;
    struct CustomErrorMsg jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = CustomErrorExit;

    // 将字符串转换为宽字符，以处理中文路径问题
    wchar_t wFilepath[1024];
    MultiByteToWideChar(CP_UTF8, 0, filepath.c_str(), -1, wFilepath, sizeof(wFilepath) / sizeof(*wFilepath));
    const wchar_t* wMode = L"rb";
    FILE* infile;
    if (0 != _wfopen_s(&infile, wFilepath, wMode)) {
        infile = nullptr;
        return Result::ERROR_OPEN_FILE;
    }

    // 设置跳转点
    if (setjmp(jerr.setjmpBuffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return Result::UNKNOWN_ERROR;
    }

    // 初始化
    jpeg_create_decompress(&cinfo);
    // 指定输入文件
    jpeg_stdio_src(&cinfo, infile);
    // 读取JPEG文件头
    jpeg_read_header(&cinfo, TRUE);
    // 开始解压缩JPEG文件
    jpeg_start_decompress(&cinfo);
    // 获取JPEG图像的宽、高、通道数
    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components;

    int rowStride = cinfo.output_width * cinfo.output_components;
    unsigned char* data = (unsigned char*)malloc(cinfo.output_height * rowStride);

    JSAMPROW rowPointer[1];
    // 逐行读取图像数据
    while (cinfo.output_scanline < cinfo.output_height) {
        rowPointer[0] = &data[cinfo.output_scanline * rowStride];
        jpeg_read_scanlines(&cinfo, rowPointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    uint64_t byteSize = width * height * channels;
    // 将图像数据复制到堆上
    if (imageData == nullptr) {
        imageData = std::make_shared<ImageData>(width, height, channels);
        imageData->orientation = orientation;
        memcpy(imageData->pixels, data, byteSize);
    }

    free(data);

    return Result::SUCCESS;
}

Result JpegReader::ParseExif(
    const std::string&                                  filename,
    std::vector<std::pair<std::string, std::string>>&   exif,
    Orientation&                                        orientation)
{

    return Result::SUCCESS;
}

JpegImageResponse::JpegImageResponse(const QString& filepath)
    : m_filepath(filepath)
{
    // 移除URL参数部分（如?async等）
    int paramIndex = m_filepath.indexOf('?');
    if (paramIndex != -1) {
        m_filepath = m_filepath.left(paramIndex);
    }

    // 移除"file:///"前缀
    if (m_filepath.startsWith("file:///")) {
        m_filepath = m_filepath.mid(8); 
    }

    setAutoDelete(false);
}

JpegImageResponse::~JpegImageResponse()
{
    qDebug() << "des";
}

void JpegImageResponse::run()
{
    // 解码文件数据
    struct jpeg_decompress_struct cinfo;
    struct CustomErrorMsg jerr;

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = CustomErrorExit;

    // 将字符串转换为宽字符，以处理中文路径问题
    wchar_t wFilepath[1024];
    MultiByteToWideChar(CP_UTF8, 0, m_filepath.toStdString().c_str(), -1, wFilepath, sizeof(wFilepath) / sizeof(*wFilepath));
    const wchar_t* wMode = L"rb";
    FILE* infile;
    if (0 != _wfopen_s(&infile, wFilepath, wMode)) {
        infile = nullptr;
        qDebug() << "error";
        return;
    }

    // 设置跳转点
    if (setjmp(jerr.setjmpBuffer)) {
        qDebug() << "error occured";
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return;
    }

    // 初始化
    jpeg_create_decompress(&cinfo);
    // 指定输入文件
    jpeg_stdio_src(&cinfo, infile);
    // 读取JPEG文件头
    jpeg_read_header(&cinfo, TRUE);
    // 开始解压缩JPEG文件
    jpeg_start_decompress(&cinfo);
    // 获取JPEG图像的宽、高、通道数
    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components;

    int rowStride = cinfo.output_width * cinfo.output_components;     // 每行字节数
    unsigned char* data = (unsigned char*)malloc(cinfo.output_height * rowStride);

    JSAMPROW rowPointer[1];
    // 逐行读取图像数据
    while (cinfo.output_scanline < cinfo.output_height) {
        rowPointer[0] = &data[cinfo.output_scanline * rowStride];
        jpeg_read_scanlines(&cinfo, rowPointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    QImage target;
    if (cinfo.out_color_space == JCS_RGB) {
        target = QImage(data, width, height, QImage::Format_RGB888);
    } else if (cinfo.out_color_space = JCS_EXT_RGBA) {
        target = QImage(data, width, height, QImage::Format_RGBA8888);
    }
    QImage imageCopy = target.copy();
    free(data);

    QMutexLocker locker(&m_mutex);
    m_image = std::move(imageCopy);
    emit finished();
}

QQuickTextureFactory* JpegImageResponse::textureFactory() const
{
    QMutexLocker locker(&m_mutex);
    return m_image.isNull() ? nullptr : QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString JpegImageResponse::errorString() const
{
    QMutexLocker locker(&m_mutex);
    return m_error;
}

QQuickImageResponse* JpegImageProvider::requestImageResponse(
    const QString&      id,
    const QSize&        requestedSize)
{
    JpegImageResponse* response = new JpegImageResponse(id);

    // 在线程池中执行解码任务
    QThreadPool::globalInstance()->start(response);

    return response;
}
