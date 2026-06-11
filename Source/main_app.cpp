#include "cpcapp.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    CPCAPP app;

    if (!app.powerOn()) {
        return -1;
    }

    app.run();

    return 0;
}