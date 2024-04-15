#ifndef _HTTP_VERSION_H_
#define _HTTP_VERSION_H_

namespace srv {
    enum class HttpVersion {
        HTTP_0_9,
        HTTP_1_0,
        HTTP_1_1,
        HTTP_2_0,
        HTTP_3_0
    };
} // namespace srv


#endif  // _HTTP_VERSION_H_
