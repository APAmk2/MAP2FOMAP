/*
MAP2FOMAP - A Fallout map TO Fonline text map converter
Copyright (C) 2025 APAMk2

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "FallMap.h"
#include "FallProto.h"
#include <filesystem>

const std::string ProtoTypeNames[6] = { "Items", "Critters", "Scenery", "Walls", "Tiles", "Misc" };

std::vector<std::vector<std::string>> ProtoLsts;
bool LstsReady = false;
uint32_t MapVer = 0;

FallMapTile_t::FallMapTile_t(ByteReader* reader)
{
	*reader >> RoofId;
	*reader >> TileId;
}

void skip_scripts(ByteReader* reader)
{
	for (size_t script_section = 0; script_section < 5; script_section++) 
	{
		uint32_t script_section_count = reader->u32();
		if (script_section_count > 0) 
		{
			uint32_t loop = script_section_count;
			if (script_section_count % 16 > 0) 
			{
				loop += 16 - script_section_count % 16;
			}

			uint32_t check = 0;
			for (size_t j = 0; j < loop; j++) 
			{
				int32_t pid = reader->i32();
				reader->i32();

				pid = (pid & 0xff000000) >> 24;

				switch (pid)
				{
				case 0:
					break;
				case 1:
					reader->u32();
					reader->u32();
					break;
				case 2:
					reader->u32();
					break;
				case 3:
				case 4:
					break;
				default:
					printf("Unknown script PID = %i\n", pid);
					break;
				}
				for (uint8_t i = 0; i < 14; i++)
				{
					reader->u32();
				}

				if ((j % 16) == 15) 
				{
					uint32_t cur_check = reader->u32();
					check += cur_check;
					reader->u32();
				}
			}
		}
	}
}

void SetupProtoLsts()
{
	ProtoLsts.resize(6);
	for (int i = 0; i < 6; i++)
	{
		std::string currString;
		std::filesystem::path path = ConvOptions.FalloutPath + "/data/proto/" + ProtoTypeNames[i] + "/" + ProtoTypeNames[i] + ".LST";
		std::ifstream input(path);
		if (input.is_open())
		{
			while (getline(input, currString))
			{
				ProtoLsts[i].push_back(currString);
			}
		}
		input.close();
	}
}

FallMapCritObject_t::FallMapCritObject_t(ByteReader* reader)
{
	*reader >> PlayerReact;
	*reader >> CurrMP;
	*reader >> CombatResult;
	*reader >> DmgLastTurn;
	*reader >> AiPack;
	*reader >> GroupId;
	*reader >> Enemy;
	*reader >> CurrHP;
	*reader >> CurrRAD;
	*reader >> CurrPoison;
}

FallMapAmmoObject_t::FallMapAmmoObject_t(ByteReader* reader)
{
	*reader >> CurrAmmo;
}

FallMapKeyObject_t::FallMapKeyObject_t(ByteReader* reader)
{
	*reader >> KeyCode;
}

FallMapMiscObject_t::FallMapMiscObject_t(ByteReader* reader)
{
	*reader >> Charges;
}

FallMapWeapObject_t::FallMapWeapObject_t(ByteReader* reader)
{
	*reader >> AmmoCount;
	*reader >> AmmoPid;
}

FallMapLadderObject_t::FallMapLadderObject_t(ByteReader* reader)
{
	*reader >> DestHex;
	if (MapVer == 20)
	{
		*reader >> DestMap;
	}
}

FallMapStairsObject_t::FallMapStairsObject_t(ByteReader* reader)
{
	*reader >> DestHex;
	*reader >> DestMap;
}

FallMapDoorObject_t::FallMapDoorObject_t(ByteReader* reader)
{
	*reader >> WalkThru;
}

FallMapElevObject_t::FallMapElevObject_t(ByteReader* reader)
{
	*reader >> ElevType;
	*reader >> ElevLevel;
}

FallMapGridObject_t::FallMapGridObject_t(ByteReader* reader)
{
	*reader >> ToMapId;
	*reader >> ChosPos;
	*reader >> MapElev;
	*reader >> ChosDir;
}

FallMapObject_t::FallMapObject_t(ByteReader* reader)
{
	reader->u32();
	*reader >> ObjectPos;
	reader->u32();
	reader->u32();
	reader->u32();
	reader->u32();
	*reader >> FrameNum;
	*reader >> Dir;
	*reader >> FIDType;
	reader->u8();
	*reader >> FIDNum;
	*reader >> Flags;
	*reader >> MapElev;
	*reader >> PIDType;
	reader->u8();
	*reader >> PIDNum;
	*reader >> CritIndex;
	*reader >> LightRadius;
	*reader >> LightIntense;
	*reader >> OutlineColor;
	reader->u32();
	*reader >> ScriptID;
	*reader >> InvenSize;
	*reader >> CritInvenSlots;
	reader->u32();
	reader->u32();

	if (PIDType == PROTO_CRITTER)
	{
		CritData = new FallMapCritObject_t(reader);
	}

	if (PIDType == PROTO_ITEM)
	{
		FallProto_t* file = nullptr;
		ByteReader* protoReader = new ByteReader;
		std::filesystem::path path = ConvOptions.FalloutPath + "/data/proto/" + ProtoTypeNames[PIDType] + "/" + ProtoLsts[PIDType][PIDNum - 1];
		if (!protoReader->Reset(path.string(), ByteReader::BigEndian)) return;
		file = new FallProto_t(protoReader);
		file->Filename = ProtoLsts[PIDType][PIDNum - 1];
		protoReader->Close();
		delete protoReader;

		if (file->ItemPro->AmmoData != nullptr)
		{
			AmmoData = new FallMapAmmoObject_t(reader);
		}

		if (file->ItemPro->KeyData != nullptr)
		{
			KeyData = new FallMapKeyObject_t(reader);
		}

		if (file->ItemPro->MiscData != nullptr)
		{
			MiscData = new FallMapMiscObject_t(reader);
		}

		if (file->ItemPro->WeapData != nullptr)
		{
			WeapData = new FallMapWeapObject_t(reader);
		}

		delete file;
	}

	if (PIDType == PROTO_SCENERY)
	{
		FallProto_t* file = nullptr;
		ByteReader* protoReader = new ByteReader;
		std::filesystem::path path = ConvOptions.FalloutPath + "/data/proto/" + ProtoTypeNames[PIDType] + "/" + ProtoLsts[PIDType][PIDNum - 1];
		if (!protoReader->Reset(path.string(), ByteReader::BigEndian)) return;
		file = new FallProto_t(protoReader);
		file->Filename = ProtoLsts[PIDType][PIDNum - 1];
		protoReader->Close();
		delete protoReader;

		if (file->ScenPro->LadderData != nullptr)
		{
			LadderData = new FallMapLadderObject_t(reader);
		}

		if (file->ScenPro->DoorData != nullptr)
		{
			DoorData = new FallMapDoorObject_t(reader);
		}

		if (file->ScenPro->StairData != nullptr)
		{
			StairsData = new FallMapStairsObject_t(reader);
		}

		if (file->ScenPro->ElevData != nullptr)
		{
			ElevData = new FallMapElevObject_t(reader);
		}

		delete file;
	}

	if (PIDType == PROTO_MISC)
	{
		if (PIDNum >= 16 && PIDNum <= 23)
		{
			GridData = new FallMapGridObject_t(reader);
		}
	}
}

FallMap_t::FallMap_t(ByteReader* reader)
{
	Version = reader->u32();
	MapVer = Version;
	if (!LstsReady)
	{
		SetupProtoLsts();
		LstsReady = true;
	}
	MapName = reader->string(16);
	*reader >> DefChosPos;
	*reader >> DefChosElv;
	*reader >> DefChosDir;
	*reader >> LVarsNum;
	*reader >> ScriptId;
	*reader >> MapFlags;
	*reader >> MapDark;
	*reader >> GVarsNum;
	*reader >> MapId;
	*reader >> EpochTime;

	for (int32_t i = 0; i < LVarsNum; i++)
	{
		LVars.push_back(reader->i32());
	}

	for (int32_t i = 0; i < GVarsNum; i++)
	{
		GVars.push_back(reader->i32());
	}

	for (uint8_t i = 0; i < 44; i++)
	{
		reader->i32();
	}

	int tileLen = 10000;
	int elevation = 1;
	switch (MapFlags & 0xE)
	{
	case 0x0c:
		tileLen = 10000;
		elevation = 1;
		break;
	case 0x08:
		tileLen = 20000;
		elevation = 2;
		break;
	case 0x00:
		tileLen = 30000;
		elevation = 3;
		break;
	}

	for (int i = 0; i < tileLen; i++)
	{
		FallMapTile_t currTile(reader);
		Tiles.push_back(currTile);
	}

	skip_scripts(reader);

	*reader >> TotalObjects;
	for (size_t elev = 0; elev < elevation; elev++) 
	{
		uint32_t objectsOnElevation = reader->u32();

		for (size_t j = 0; j < objectsOnElevation; j++)
		{
			FallMapObject_t newObj(reader);
			for (size_t i = 0; i < newObj.InvenSize; i++)
			{
				uint32_t amount = reader->u32();
				FallMapObject_t subObj(reader);
				Objects.push_back(subObj);
			}
			Objects.push_back(newObj);
		}
	}
}