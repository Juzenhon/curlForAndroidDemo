//
// Created by juzenhon on 2018/2/9.
//

#ifndef LIBCURLDEMO_REQUESTBODY_H
#define LIBCURLDEMO_REQUESTBODY_H

#include "MediaType.h"

namespace okhttp{

    class RequestBody {

    public:

        virtual string getContentType() const;

        virtual  int getContentLength() const;

        virtual void writeTo(const char* ) = 0;

    };
}




#endif //LIBCURLDEMO_REQUESTBODY_H
