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

#include "FallProto.h"

ItemArmorProto_t::ItemArmorProto_t(ByteReader* reader)
{
	*reader >> AC;
	for (size_t i = 0; i < 7; i++)
	{
		*reader >> Resists[i];
	}

	for (size_t i = 0; i < 7; i++)
	{
		*reader >> Thresholds[i];
	}

	*reader >> Perk;
	MaleCRType = new PID_t(reader);
	FemaleCRType = new PID_t(reader);
}

ItemContProto_t::ItemContProto_t(ByteReader* reader)
{
	*reader >> MaxVolume;
	*reader >> ContFlags;
}

ItemDrugProto_t::ItemDrugProto_t(ByteReader* reader)
{
	for (size_t i = 0; i < 3; i++)
	{
		*reader >> Stats[i];
	}
	for (size_t i = 0; i < 3; i++)
	{
		*reader >> FirstMod[i];
	}
	*reader >> Duration1;
	for (size_t i = 0; i < 3; i++)
	{
		*reader >> SecondMod[i];
	}
	*reader >> Duration2;
	for (size_t i = 0; i < 3; i++)
	{
		*reader >> ThirdMod[i];
	}
	*reader >> AddictRate;
	*reader >> AddictEffect;
	*reader >> AddictDelay;
}

ItemWeapProto_t::ItemWeapProto_t(ByteReader* reader)
{
	*reader >> AnimCode;
	*reader >> MinDmg;
	*reader >> MaxDmg;
	*reader >> DmgType;
	*reader >> MaxRange1;
	*reader >> MaxRange2;
	ProjPID = new PID_t(reader);
	*reader >> MinST;
	*reader >> APCost1;
	*reader >> APCost2;
	*reader >> CritFail;
	*reader >> Perk;
	*reader >> Rounds;
	*reader >> Caliber;
	AmmoPID = new PID_t(reader);
	*reader >> MaxAmmo;
	*reader >> SoundId;
}

ItemAmmoProto_t::ItemAmmoProto_t(ByteReader* reader)
{
	*reader >> Caliber;
	*reader >> Quantity;
	*reader >> ACMod;
	*reader >> DRMod;
	*reader >> DmgMult;
	*reader >> DmgDiv;
}

ItemMiscProto_t::ItemMiscProto_t(ByteReader* reader)
{
	PowerPID = new PID_t(reader);
	*reader >> PowerType;
	*reader >> Charges;
}

ItemKeyProto_t::ItemKeyProto_t(ByteReader* reader)
{
	*reader >> KeyID;
}

ItemProto_t::ItemProto_t(ByteReader* reader)
{
	*reader >> FlagsExt;
	*reader >> ScriptId;
	*reader >> Subtype;
	*reader >> Material;
	*reader >> Volume;
	*reader >> Weight;
	*reader >> Cost;
	InvFID = new PID_t(reader);
	*reader >> SoundID;

	if (Subtype == ITEM_TYPE_ARMOR)
	{
		ArmorData = new ItemArmorProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_CONTAINER)
	{
		ContData = new ItemContProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_DRUG)
	{
		DrugData = new ItemDrugProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_WEAPON)
	{
		WeapData = new ItemWeapProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_AMMO)
	{
		AmmoData = new ItemAmmoProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_MISC)
	{
		MiscData = new ItemMiscProto_t(reader);
	}
	else if (Subtype == ITEM_TYPE_KEY)
	{
		KeyData = new ItemKeyProto_t(reader);
	}
}

CritterProto_t::CritterProto_t(ByteReader* reader)
{
	*reader >> ActionFlags;
	ScriptID = new PID_t(reader);
	HeadFID = new PID_t(reader);
	*reader >> AiID;
	*reader >> TeamID;
	*reader >> CritFlags;
	for (size_t i = 0; i < 7; i++)
	{
		*reader >> SPECIAL[i];
	}
	*reader >> HP;
	*reader >> AP;
	*reader >> AC;
	*reader >> UnarmedDMG;
	*reader >> MeleeDMG;
	*reader >> CarryWeight;
	*reader >> Sequence;
	*reader >> HealRate;
	*reader >> CritChance;
	*reader >> BetterCrits;
	for (size_t i = 0; i < 7; i++)
	{
		*reader >> Thresholds[i];
	}
	for (size_t i = 0; i < 9; i++)
	{
		*reader >> Resists[i];
	}
	*reader >> Age;
	*reader >> Sex;
	for (size_t i = 0; i < 7; i++)
	{
		*reader >> BonusSPECIAL[i];
	}
	*reader >> BonusHP;
	*reader >> BonusAP;
	*reader >> BonusAC;
	*reader >> BonusUnarmedDMG;
	*reader >> BonusMeleeDMG;
	*reader >> BonusCarryWeight;
	*reader >> BonusSequence;
	*reader >> BonusHealRate;
	*reader >> BonusCritChance;
	*reader >> BonusBetterCrits;
	for (size_t i = 0; i < 7; i++)
	{
		*reader >> BonusThresholds[i];
	}
	for (size_t i = 0; i < 9; i++)
	{
		*reader >> BonusResists[i];
	}
	*reader >> BonusAge;
	*reader >> BonusSex;
	for (size_t i = 0; i < 18; i++)
	{
		*reader >> Skills[i];
	}
	*reader >> BodyType;
	*reader >> ExpVal;
	*reader >> KillType;
	*reader >> DmgType;
}

ScenDoorProto_t::ScenDoorProto_t(ByteReader* reader)
{
	*reader >> WalkThru;
	*reader >> DoorFlag;
}

ScenStairProto_t::ScenStairProto_t(ByteReader* reader)
{
	*reader >> DestElev;
	reader->u8();
	*reader >> DestTile;
	*reader >> DestMap;
}

ScenElevProto_t::ScenElevProto_t(ByteReader* reader)
{
	*reader >> ElevType;
	*reader >> ElevLevel;
}

ScenLadderProto_t::ScenLadderProto_t(ByteReader* reader)
{
	*reader >> DestElev;
	reader->u8();
	*reader >> DestTile;
}

ScenGenericProto_t::ScenGenericProto_t(ByteReader* reader)
{
	*reader >> Unknown;
}

ScenProto_t::ScenProto_t(ByteReader* reader)
{
	*reader >> WallLightFlags;
	*reader >> ActionFlags;
	ScriptID = new PID_t(reader);
	*reader >> SubType;
	*reader >> Material;
	*reader >> SoundID;

	if (SubType == SCEN_TYPE_DOOR)
	{
		DoorData = new ScenDoorProto_t(reader);
	}
	else if (SubType == SCEN_TYPE_STAIR)
	{
		StairData = new ScenStairProto_t(reader);
	}
	else if (SubType == SCEN_TYPE_ELEV)
	{
		ElevData = new ScenElevProto_t(reader);
	}
	else if (SubType == SCEN_TYPE_LADDER_BTM || SubType == SCEN_TYPE_LADDER_TOP)
	{
		LadderData = new ScenLadderProto_t(reader);
	}
	else if (SubType == SCEN_TYPE_GENERIC)
	{
		GenericData = new ScenGenericProto_t(reader);
	}
}

WallProto_t::WallProto_t(ByteReader* reader)
{
	*reader >> WallLightFlags;
	*reader >> ActionFlags;
	ScriptID = new PID_t(reader);
	*reader >> Material;
}

TileProto_t::TileProto_t(ByteReader* reader)
{
	*reader >> Material;
}

FallProto_t::FallProto_t(ByteReader* reader)
{
	PID = new PID_t(reader);
	*reader >> TextId;
	FID = new PID_t(reader);
	*reader >> LightRad;
	*reader >> LightIntence;
	*reader >> Flags;

	if (PID->PIDType == PROTO_ITEM)
	{
		ItemPro = new ItemProto_t(reader);
	}
	if (PID->PIDType == PROTO_CRITTER)
	{
		CritPro = new CritterProto_t(reader);
	}
	if (PID->PIDType == PROTO_SCENERY)
	{
		ScenPro = new ScenProto_t(reader);
	}
	if (PID->PIDType == PROTO_WALLS)
	{
		WallPro = new WallProto_t(reader);
	}
	if (PID->PIDType == PROTO_TILES)
	{
		TilePro = new TileProto_t(reader);
	}
}