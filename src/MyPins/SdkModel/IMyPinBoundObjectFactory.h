// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#pragma once

#include <string>
#include "Types.h"
#include "MyPins.h"
#include "Search.h"
#include "MyPinModel.h"
#include "MyPinsSemanticPinType.h"

namespace ExampleApp
{
    namespace MyPins
    {
        namespace SdkModel
        {
            class IMyPinBoundObjectFactory
            {
            public:
                virtual ~IMyPinBoundObjectFactory() { }
                
                virtual IMyPinBoundObject* CreateUserCreatedPinBoundObject(MyPinsFileIO& myPinsFileIO,
                                                                           MyPinModel::TPinIdType pinId,
                                                                           Byte* imageData,
                                                                           size_t imageSize,
                                                                           bool share) = 0;
                
                virtual IMyPinBoundObject* CreateSearchResultPinBoundObject(MyPinsFileIO& myPinsFileIO,
                                                                            MyPinModel::TPinIdType pinId,
                                                                            const Search::SdkModel::SearchResultModel& searchResult) = 0;
                
                virtual IMyPinBoundObject* CreatePinBoundObjectFromSerialized(MyPinsFileIO& myPinsFileIO,
                                                                              MyPinModel::TPinIdType pinId,
                                                                              const MyPinsSemanticPinType& semanticPinType,
                                                                              const std::string& serializedData) = 0;
            };
        }
    }
}
