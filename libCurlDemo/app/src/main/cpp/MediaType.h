//
// Created by juzenhon on 2018/2/9.
//

#ifndef LIBCURLDEMO_MEDIATYPE_H
#define LIBCURLDEMO_MEDIATYPE_H

#include <string>
#include <regex>

namespace okhttp {

    using namespace std;

    class MediaType {

    public:
        explicit MediaType(const char *mediaType) : _mediaType(mediaType) {
//            std::smatch mdata;
//            std::regex_match(mediaType, mdata, TYPE_SUBTYPE);
//            std::smatch::iterator x;
//            int pos = 0;
//            for (auto iter = mdata.begin(); iter != mdata.end(); ++iter) {
//                if (pos == 0) {
//                    _type = iter->str();
//                } else {
//                    _subType = iter->str();
//                }
//                ++pos;
//            }

        }

        MediaType(const char *type, const char *subType, const char *charset)
                : _type(type), _subType(subType), _charset(charset) {

            __glibcxx_assert(type)
            __glibcxx_assert(subType)
            _mediaType/*.append("Content-Type: ")*/.append(type).append("/").append(subType)
                    .append(";").append("charset=");
            if (charset) {
                _mediaType.append(charset);
            } else {
                _mediaType.append("utf-8");
            }

        }

        string &getContentType() const;

        /**
       * Returns the charset of this media type, or null if this media type doesn't specify a charset.
       */
        string &getCharset() const;

        /**
       * Returns the high-level media type, such as "text", "image", "audio", "video", or
       * "application".
       */
        string &getType() const;

        /**
       * Returns a specific media subtype, such as "plain" or "png", "mpeg", "mp4" or "xml".
       */
        string &getSubType() const;

    private:
        static const string TOKEN;
        static const string QUOTED ;
        static const regex TYPE_SUBTYPE;
        static const regex PARAMETER;

        string _mediaType;
        string _type;
        string _subType;
        string _charset;
    };

    const string MediaType::TOKEN = "([a-zA-Z0-9-!#$%&'*+.^_`{|}~]+)";
    const string MediaType::QUOTED = "\"([^\"]*)\"";
    const regex MediaType::TYPE_SUBTYPE = regex(MediaType::TOKEN + "/" + MediaType::TOKEN);
    const regex MediaType::PARAMETER = regex(";\\s*(?:" + MediaType::TOKEN + "=(?:" + MediaType::TOKEN + "|" + MediaType::QUOTED + "))?");

}
#endif //LIBCURLDEMO_MEDIATYPE_H
