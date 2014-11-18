// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#pragma once

#include <map>
#include "Types.h"
#include "Pins.h"
#include "PinController.h"
#include "WorldPins.h"
#include "IWorldPinsService.h"
#include "VectorMath.h"
#include "EnvironmentFlatteningService.h"

namespace ExampleApp
{
	namespace WorldPins
	{
		class WorldPinsService : public IWorldPinsService, private Eegeo::NonCopyable
		{
			typedef std::map<WorldPinItemModel::WorldPinItemModelId, IWorldPinSelectionHandler*>::iterator mapIt;

			std::map<WorldPinItemModel::WorldPinItemModelId, IWorldPinSelectionHandler*> m_pinsToSelectionHandlers;

			IWorldPinsRepository& m_worldPinsRepository;
			IWorldPinsFactory& m_worldPinsFactory;
			Eegeo::Pins::PinRepository& m_pinRepository;
			Eegeo::Pins::PinController& m_pinController;
			const Eegeo::Rendering::EnvironmentFlatteningService& m_environmentFlatteningService;

		public:
			WorldPinsService(IWorldPinsRepository& worldPinsRepository,
			                 IWorldPinsFactory& worldPinsFactory,
			                 Eegeo::Pins::PinRepository& pinRepository,
			                 Eegeo::Pins::PinController& pinController,
			                 const Eegeo::Rendering::EnvironmentFlatteningService& environmentFlatteningService);

			~WorldPinsService();

			WorldPinItemModel* AddPin(IWorldPinSelectionHandler* pSelectionHandler,
                                      const WorldPinFocusData& worldPinFocusData,
                                      const Eegeo::Space::LatLong& location,
                                      int iconIndex);

			void RemovePin(WorldPinItemModel* pinItemModel);

			void UpdatePinScale(const WorldPinItemModel& pinItemModel, float scale);

			bool HandleTouchTap(const Eegeo::v2& screenTapPoint);

			void GetPinEcefAndScreenLocations(const WorldPinItemModel& pinItemModel,
			                                  Eegeo::dv3& ecefLocation,
			                                  Eegeo::v2& screenLocation) const;
            
            IWorldPinSelectionHandler* GetSelectionHandlerForPin(WorldPinItemModel::WorldPinItemModelId worldPinItemModelId);

		private:
			void ErasePin(const WorldPinItemModel::WorldPinItemModelId& id);

		};
	}
}