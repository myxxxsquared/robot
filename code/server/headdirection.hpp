
#include <thread>
#include <mutex>

namespace cv
{
    class Mat;
}
class SocketPipe;

class HeadDirection
{
  public:
    SocketPipe *pipe;
    std::thread thread_capture;
    std::thread thread_process;
    std::mutex mutex_frame;
    cv::Mat *volatile curframe;
    volatile bool started;

    HeadDirection(SocketPipe *p);

    HeadDirection(const HeadDirection &) = delete;
    HeadDirection &operator=(const HeadDirection &) = delete;

    void start();
    void capture_proc();
    void process_proc();

    static void capture_proc_static(HeadDirection *obj);
    static void process_proc_static(HeadDirection *obj);
};

