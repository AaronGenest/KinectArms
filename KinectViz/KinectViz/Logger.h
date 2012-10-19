#ifndef LOGGER_H
#define LOGGER_H


#define LOG_TAG "KinectTable"
#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)
#define LOGD(...) printf("D/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__); printf("\n")
#define LOGI(...) printf("I/" LOG_TAG "(" ")" __VA_ARGS__); printf("\n")
#define LOGE(...) fprintf(stderr, "E/" LOG_TAG "(" ")" __VA_ARGS__); printf("\n")


#endif