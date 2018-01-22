
#include "datatypes.hpp"

class Dialog
{
public:
    SAFE_DEQUE<std::string> *queue_instr;
    SAFE_DEQUE<std::string> *queue_out;

    static const int UID_LENGTH = 32;
    char USERID[UID_LENGTH + 1];

    Dialog(SAFE_DEQUE<std::string> *qi, SAFE_DEQUE<std::string> *qo);

    void dialog();
    void do_dialog();
};
