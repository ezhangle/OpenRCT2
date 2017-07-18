#include "PlaceParkEntranceAction.hpp"
#include "SetParkEntranceFeeAction.hpp"
#include "RideCreateAction.hpp"
#include "RideSetStatus.hpp"

extern "C"
{
#pragma region PlaceParkEntranceAction
    money32 place_park_entrance(sint16 x, sint16 y, sint16 z, uint8 direction)
    {
        auto gameAction = PlaceParkEntranceAction();
        gameAction.x = x;
        gameAction.y = y;
        gameAction.z = z;
        gameAction.direction = direction;
        auto result = GameActions::Execute(&gameAction);
        if (result.Error == GA_ERROR::OK)
        {
            return 0;
        }
        else
        {
            return MONEY32_UNDEFINED;
        }
    }

    /**
    *
    *  rct2: 0x006666E7
    */
    void game_command_place_park_entrance(sint32* eax,
                                          sint32* ebx,
                                          sint32* ecx,
                                          sint32* edx,
                                          sint32* esi,
                                          sint32* edi,
                                          sint32* ebp)
    {
        Guard::Assert(false, "GAME_COMMAND_PLACE_PARK_ENTRANCE DEPRECIATED");
    }

    /**
    *
    *  rct2: 0x00666F4E
    */
    money32 park_entrance_place_ghost(sint32 x, sint32 y, sint32 z, sint32 direction)
    {
        park_entrance_remove_ghost();

        auto gameAction = PlaceParkEntranceAction();
        gameAction.x = x;
        gameAction.y = y;
        gameAction.z = z;
        gameAction.direction = direction;
        gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

        auto result = GameActions::Execute(&gameAction);
        if (result.Error == GA_ERROR::OK)
        {
            gParkEntranceGhostPosition.x = x;
            gParkEntranceGhostPosition.y = y;
            gParkEntranceGhostPosition.z = z;
            gParkEntranceGhostDirection = direction;
            gParkEntranceGhostExists = true;
        }
        return result.Cost;
    }
#pragma endregion

#pragma region SetParkEntranceFeeAction
    void park_set_entrance_fee(money32 value)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (money16)value;
        GameActions::Execute(&gameAction);
    }

    void game_command_set_park_entrance_fee(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (*edi & 0xFFFF);
        GameActions::Execute(&gameAction);
    }
#pragma endregion

#pragma region RideCreateAction
    /**
    *
    *  rct2: 0x006B4800
    */
    void ride_construct_new(ride_list_item listItem)
    {
        auto gameAction = RideCreateAction();
        gameAction.rideType = listItem.type;
        gameAction.rideSubType = listItem.entry_index;
        gameAction.SetCallback([](const IGameAction *ga, GameActionResult& res)
        {
            if (res.Error != GA_ERROR::OK)
                return;

            ride_construct(static_cast<RideCreateGameActionResult&>(res).RideIndex());
        });

        GameActions::Execute(&gameAction);
    }

    /**
    *
    *  rct2: 0x006B3F0F
    */
    void game_command_create_ride(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
    {
        ride_list_item tmp;
        tmp.ride_type_and_entry = (uint16)*edx;
        ride_construct_new(tmp);
    }

#pragma endregion

#pragma region RideSetStatusAction

    void ride_set_status(sint32 rideIndex, sint32 status)
    {
        game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, rideIndex | (status << 8), GAME_COMMAND_SET_RIDE_STATUS, 0, 0);

        auto gameAction = RideSetStatusAction();
        gameAction.RideIndex = rideIndex;
        gameAction.Status = status;

        GameActions::Execute(&gameAction);
    }

    /**
    *
    *  rct2: 0x006B4EA6
    */
    void game_command_set_ride_status(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
    {
    }

#pragma endregion
}
