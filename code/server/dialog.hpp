
#include "safequeue.hpp"

class Dialog
{
public:
    safe_queue<std::string> *queue_instr;
    safe_queue<std::string> *queue_out;

    static const int UID_LENGTH = 32;
    char USERID[UID_LENGTH + 1];

    Dialog(safe_queue<std::string> *qi, safe_queue<std::string> *qo);

    void dialog();
    void do_dialog();
};
