#ifndef IVW_RAWVOLUMEREADER_H
#define IVW_RAWVOLUMEREADER_H

#include "inviwo/core/inviwocoredefine.h"
#include "inviwo/core/inviwo.h"
#include "inviwo/core/io/volumereader.h"

namespace inviwo {

class IVW_CORE_API RawVolumeReader {
public:
    struct ReaderSettings {            
        std::string rawFileAbsolutePath_;
        ivec3 dimensions_;
        std::string dataFormat_;
        ReaderSettings(std::string rawFile, ivec3 resolution=ivec3(0), std::string format="UCHAR");
        ReaderSettings() {}
    };

    RawVolumeReader();
    virtual ~RawVolumeReader() {}

   static  void* loadRawData(RawVolumeReader::ReaderSettings& readerSettings);

protected:
    template <class T>
    static T* loadData(std::string rawFileAbsolutePath, ivec3 dimensions) {
        T* texels = new T[dimensions.x * dimensions.y * dimensions.z];
        std::fstream fin(rawFileAbsolutePath.c_str(), std::ios::in | std::ios::binary);
        ivwAssert(fin.good(), "cannot open volume file");
        fin.read((char*)texels, dimensions.x*dimensions.y*dimensions.z);
        fin.close();
        return texels;
    }

};

} // namespace

#endif // IVW_RAWVOLUMEREADER_H
