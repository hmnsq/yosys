/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Jannis Harder <jix@yosyshq.com> <me@jix.one>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DRIVERTOOLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು DRIVERTOOLS_H

#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ರಚನೆ Drive_ಬಿಟ್;

ರಚನೆ DriveChunkWire;
ರಚನೆ Drive_Chunk_ಪೋರ್ಟ್;
ರಚನೆ DriveChunkMarker;
ರಚನೆ DriveChunk;

ರಚನೆ DriveSpec;

std::string log_signal(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk);
std::string log_signal(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk);
std::string log_signal(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk);
std::string log_signal(DriveChunk ಸ್ಥಿರಾಂಕ &chunk);
std::string log_signal(DriveSpec ಸ್ಥಿರಾಂಕ &chunk);

ಎಣಿಕೆ class DriveType : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ
{
	NONE,
	CONSTANT,
	WIRE,
	ಪೋರ್ಟ್_2,
	ಅಪವರ್ತ್ಯ,
	MARKER,
};

ರಚನೆ Drive_ಬಿಟ್_Wire
{
	Wire *wire;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	Drive_ಬಿಟ್_Wire(Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) : wire(wire), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) {}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Wire &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ wire == other.wire && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Wire &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (wire != other.wire)
			ಹಿಂತಿರುಗಿ wire->name < other.wire->name;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;


	ಕ್ರಿಯಾಕಾರಕ Sig_ಬಿಟ್() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ Sig_ಬಿಟ್(wire, ಸ್ಥಾನಾಂತರ);
	}
};

ರಚನೆ Drive_ಬಿಟ್_ಪೋರ್ಟ್
{
	Cell *cell;
	IdString ಪೋರ್ಟ್;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	Drive_ಬಿಟ್_ಪೋರ್ಟ್(Cell *cell, IdString ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) : cell(cell), ಪೋರ್ಟ್(ಪೋರ್ಟ್), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) {}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_ಪೋರ್ಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ cell == other.cell && ಪೋರ್ಟ್ == other.ಪೋರ್ಟ್ && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_ಪೋರ್ಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (cell != other.cell)
			ಹಿಂತಿರುಗಿ cell->name < other.cell->name;
		ಆದರೆ (ಪೋರ್ಟ್ != other.ಪೋರ್ಟ್)
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್ < other.ಪೋರ್ಟ್;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

};


ರಚನೆ Drive_ಬಿಟ್_Marker
{
	ಪೂರ್ಣಾಂಕ marker;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	Drive_ಬಿಟ್_Marker(ಪೂರ್ಣಾಂಕ marker, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) : marker(marker), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) {}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Marker &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ marker == other.marker && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Marker &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (marker != other.marker)
			ಹಿಂತಿರುಗಿ marker < other.marker;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

};

ರಚನೆ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ
{
ಖಾಸಗಿ:
	pool<Drive_ಬಿಟ್> ಅಪವರ್ತ್ಯ_3;

ಸಾರ್ವಜನಿಕ:
	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ();
	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single);

	pool<Drive_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }

	ಶೂನ್ಯ ವಿಲೀನ(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &other)
	{
		ಗಾಗಿ (Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single : other.ಅಪವರ್ತ್ಯ_3)
			ವಿಲೀನ(single);
	}

	ಶೂನ್ಯ ವಿಲೀನ(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ &&other)
	{
		ಗಾಗಿ (Drive_ಬಿಟ್ &single : other.ಅಪವರ್ತ್ಯ_3)
			ವಿಲೀನ(std::move(single));
	}

	ಶೂನ್ಯ ವಿಲೀನ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single);
	ಶೂನ್ಯ ವಿಲೀನ(Drive_ಬಿಟ್ &&single);

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3 == other.ಅಪವರ್ತ್ಯ_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ Drive_ಬಿಟ್
{
ಖಾಸಗಿ:
	DriveType type_ = DriveType::NONE;
	ಒಕ್ಕೂಟ
	{
		State constant_;
		Drive_ಬಿಟ್_Wire wire_;
		Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಪೋರ್ಟ್_3;
		Drive_ಬಿಟ್_Marker marker_;
		Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಅಪವರ್ತ್ಯ_3;
	};
ಸಾರ್ವಜನಿಕ:
	Drive_ಬಿಟ್()  {}

	Drive_ಬಿಟ್(Sig_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);

	Drive_ಬಿಟ್(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other) { *this = other; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್ &&other) { *this = other; }


	Drive_ಬಿಟ್(State constant) { *this = constant; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &wire) { *this = wire; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_Wire &&wire) { *this = wire; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್) { *this = ಪೋರ್ಟ್; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_ಪೋರ್ಟ್ &&ಪೋರ್ಟ್) { *this = ಪೋರ್ಟ್; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &marker) { *this = marker; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_Marker &&marker) { *this = marker; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2) { *this = ಅಪವರ್ತ್ಯ_2; }
	Drive_ಬಿಟ್(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ &&ಅಪವರ್ತ್ಯ_2) { *this = ಅಪವರ್ತ್ಯ_2; }

	~Drive_ಬಿಟ್() { ಗಣ_none(); }

	ಶೂನ್ಯ ಗಣ_none()
	{
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				wire_.~Drive_ಬಿಟ್_Wire();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಪೋರ್ಟ್_3.~Drive_ಬಿಟ್_ಪೋರ್ಟ್();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				marker_.~Drive_ಬಿಟ್_Marker();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಅಪವರ್ತ್ಯ_3.~Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ();
				ನಿಲ್ಲಿಸು;
		}
		type_ = DriveType::NONE;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other)
	{
		ಆಯ್ಕೆ (other.type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಗಣ_none();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				*this = other.constant_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				*this = other.wire_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				*this = other.ಪೋರ್ಟ್_3;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				*this = other.marker_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				*this = other.ಅಪವರ್ತ್ಯ_3;
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್ &&other)
	{
		ಆಯ್ಕೆ (other.type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಗಣ_none();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				*this = std::move(other.wire_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				*this = std::move(other.ಪೋರ್ಟ್_3);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				*this = std::move(other.marker_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				*this = std::move(other.ಅಪವರ್ತ್ಯ_3);
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(State constant)
	{
		ಗಣ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &wire)
	{
		ಗಣ_none();
		new (&wire_) Drive_ಬಿಟ್_Wire(wire);
		type_ = DriveType::WIRE;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Wire &&wire)
	{
		ಗಣ_none();
		new (&wire_) Drive_ಬಿಟ್_Wire(wire);
		type_ = DriveType::WIRE;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್)
	{
		ಗಣ_none();
		new (&ಪೋರ್ಟ್_3) Drive_ಬಿಟ್_ಪೋರ್ಟ್(ಪೋರ್ಟ್);
		type_ = DriveType::ಪೋರ್ಟ್_2;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಪೋರ್ಟ್ &&ಪೋರ್ಟ್)
	{
		ಗಣ_none();
		new (&ಪೋರ್ಟ್_3) Drive_ಬಿಟ್_ಪೋರ್ಟ್(ಪೋರ್ಟ್);
		type_ = DriveType::ಪೋರ್ಟ್_2;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &marker)
	{
		ಗಣ_none();
		new (&marker_) Drive_ಬಿಟ್_Marker(marker);
		type_ = DriveType::MARKER;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Marker &&marker)
	{
		ಗಣ_none();
		new (&marker_) Drive_ಬಿಟ್_Marker(marker);
		type_ = DriveType::MARKER;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2)
	{
		ಗಣ_none();
		ಆದರೆ (ಅಪವರ್ತ್ಯ_2.ಅಪವರ್ತ್ಯ_2().ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ *this;
		new (&ಅಪವರ್ತ್ಯ_3) Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ(ಅಪವರ್ತ್ಯ_2);
		type_ = DriveType::ಅಪವರ್ತ್ಯ;
		ಹಿಂತಿರುಗಿ *this;
	}

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ &&ಅಪವರ್ತ್ಯ_2)
	{
		ಗಣ_none();
		ಆದರೆ (ಅಪವರ್ತ್ಯ_2.ಅಪವರ್ತ್ಯ_2().ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ *this;
		new (&ಅಪವರ್ತ್ಯ_3) Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ(ಅಪವರ್ತ್ಯ_2);
		type_ = DriveType::ಅಪವರ್ತ್ಯ;
		ಹಿಂತಿರುಗಿ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (type_ != other.type_)
			ಹಿಂತಿರುಗಿ false;

		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ true;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_ == other.constant_;
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_ == other.wire_;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3 == other.ಪೋರ್ಟ್_3;
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_ == other.marker_;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3 == other.ಅಪವರ್ತ್ಯ_3;
		}
		log_abort();
	}

	bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ !(*this == other);
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (type_ != other.type_)
			ಹಿಂತಿರುಗಿ type_ < other.type_;
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ false;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_ < other.constant_;
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_ < other.wire_;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3 < other.ಪೋರ್ಟ್_3;
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_ < other.marker_;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_; }

	bool is_none() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::NONE; }
	bool is_constant() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::CONSTANT; }
	bool is_wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::WIRE; }
	bool is_ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::ಪೋರ್ಟ್_2; }
	bool is_marker() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::MARKER; }
	bool is_ಅಪವರ್ತ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::ಅಪವರ್ತ್ಯ; }

	State &constant() { log_assert(is_constant()); ಹಿಂತಿರುಗಿ constant_; }
	State ಸ್ಥಿರಾಂಕ &constant() ಸ್ಥಿರಾಂಕ { log_assert(is_constant()); ಹಿಂತಿರುಗಿ constant_; }
	Drive_ಬಿಟ್_Wire &wire() { log_assert(is_wire()); ಹಿಂತಿರುಗಿ wire_; }
	Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &wire() ಸ್ಥಿರಾಂಕ { log_assert(is_wire()); ಹಿಂತಿರುಗಿ wire_; }
	Drive_ಬಿಟ್_ಪೋರ್ಟ್ &ಪೋರ್ಟ್() { log_assert(is_ಪೋರ್ಟ್()); ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3; }
	Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { log_assert(is_ಪೋರ್ಟ್()); ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3; }
	Drive_ಬಿಟ್_Marker &marker() { log_assert(is_marker()); ಹಿಂತಿರುಗಿ marker_; }
	Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &marker() ಸ್ಥಿರಾಂಕ { log_assert(is_marker()); ಹಿಂತಿರುಗಿ marker_; }
	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ &ಅಪವರ್ತ್ಯ_2() { log_assert(is_ಅಪವರ್ತ್ಯ()); ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }
	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2() ಸ್ಥಿರಾಂಕ { log_assert(is_ಅಪವರ್ತ್ಯ()); ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }

	ಶೂನ್ಯ ವಿಲೀನ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other);

};

ಸಾಲೊಳಗೆ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ::Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ() {}
ಸಾಲೊಳಗೆ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ::Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single)
{
	ಅಪವರ್ತ್ಯ_3.emplace(single);
}

ರಚನೆ DriveChunkWire
{
	Wire *wire;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ ಅಗಲ;

	DriveChunkWire(Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ) : wire(wire), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ), ಅಗಲ(ಅಗಲ) {}
	DriveChunkWire(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್) : wire(ಬಿಟ್.wire), ಸ್ಥಾನಾಂತರ(ಬಿಟ್.ಸ್ಥಾನಾಂತರ), ಅಗಲ(1) {}

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ; }

	Drive_ಬಿಟ್_Wire ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
	{
		log_assert(i >= 0 && i < ಅಗಲ);
		ಹಿಂತಿರುಗಿ Drive_ಬಿಟ್_Wire(wire, ಸ್ಥಾನಾಂತರ + i);
	}

	bool can_append(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;
	bool try_append(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	bool try_append(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk);

	// Whether this chunk is a whole wire
	bool is_ಸಂಪೂರ್ಣ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ == 0 && ಅಗಲ == wire->ಅಗಲ; }

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ DriveChunkWire &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ wire == other.wire && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ && ಅಗಲ == other.ಅಗಲ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ DriveChunkWire &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (wire != other.wire)
			ಹಿಂತಿರುಗಿ wire->name < other.wire->name;
		ಆದರೆ (ಅಗಲ != other.ಅಗಲ)
			ಹಿಂತಿರುಗಿ ಅಗಲ < other.ಅಗಲ;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

	explicit ಕ್ರಿಯಾಕಾರಕ SigChunk() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ SigChunk(wire, ಸ್ಥಾನಾಂತರ, ಅಗಲ);
	}
};

ರಚನೆ Drive_Chunk_ಪೋರ್ಟ್
{
	Cell *cell;
	IdString ಪೋರ್ಟ್;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ ಅಗಲ;

	Drive_Chunk_ಪೋರ್ಟ್(Cell *cell, IdString ಪೋರ್ಟ್, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ) :
		cell(cell), ಪೋರ್ಟ್(ಪೋರ್ಟ್), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ), ಅಗಲ(ಅಗಲ) { }
	Drive_Chunk_ಪೋರ್ಟ್(Cell *cell, IdString ಪೋರ್ಟ್) :
		cell(cell), ಪೋರ್ಟ್(ಪೋರ್ಟ್), ಸ್ಥಾನಾಂತರ(0), ಅಗಲ(Get_ಗಾತ್ರ(cell->connections().at(ಪೋರ್ಟ್))) { }
	Drive_Chunk_ಪೋರ್ಟ್(Cell *cell, std::pair<IdString, SigSpec> ಸ್ಥಿರಾಂಕ &conn) :
		cell(cell), ಪೋರ್ಟ್(conn.ಮೊದಲ), ಸ್ಥಾನಾಂತರ(0), ಅಗಲ(Get_ಗಾತ್ರ(conn.ಎರಡನೇ)) { }
	Drive_Chunk_ಪೋರ್ಟ್(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) :
		cell(ಬಿಟ್.cell), ಪೋರ್ಟ್(ಬಿಟ್.ಪೋರ್ಟ್), ಸ್ಥಾನಾಂತರ(ಬಿಟ್.ಸ್ಥಾನಾಂತರ), ಅಗಲ(1) { }

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ; }

	Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
	{
		log_assert(i >= 0 && i < ಅಗಲ);
		ಹಿಂತಿರುಗಿ Drive_ಬಿಟ್_ಪೋರ್ಟ್(cell, ಪೋರ್ಟ್, ಸ್ಥಾನಾಂತರ + i);
	}

	bool can_append(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;
	bool try_append(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	bool try_append(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk);

	// Whether this chunk is a whole port
	bool is_ಸಂಪೂರ್ಣ() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ == 0 && ಅಗಲ == cell->connections().at(ಪೋರ್ಟ್).ಗಾತ್ರ_2();
	}

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_Chunk_ಪೋರ್ಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ cell == other.cell && ಪೋರ್ಟ್ == other.ಪೋರ್ಟ್ && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ && ಅಗಲ == other.ಅಗಲ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_Chunk_ಪೋರ್ಟ್ &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (cell != other.cell)
			ಹಿಂತಿರುಗಿ cell->name < other.cell->name;
		ಆದರೆ (ಪೋರ್ಟ್ != other.ಪೋರ್ಟ್)
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್ < other.ಪೋರ್ಟ್;
		ಆದರೆ (ಅಗಲ != other.ಅಗಲ)
			ಹಿಂತಿರುಗಿ ಅಗಲ < other.ಅಗಲ;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};


ರಚನೆ DriveChunkMarker
{
	ಪೂರ್ಣಾಂಕ marker;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
	ಪೂರ್ಣಾಂಕ ಅಗಲ;

	DriveChunkMarker(ಪೂರ್ಣಾಂಕ marker, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ) :
		marker(marker), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ), ಅಗಲ(ಅಗಲ) {}
	DriveChunkMarker(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್) :
		marker(ಬಿಟ್.marker), ಸ್ಥಾನಾಂತರ(ಬಿಟ್.ಸ್ಥಾನಾಂತರ), ಅಗಲ(1) {}

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ; }

	Drive_ಬಿಟ್_Marker ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
	{
		log_assert(i >= 0 && i < ಅಗಲ);
		ಹಿಂತಿರುಗಿ Drive_ಬಿಟ್_Marker(marker, ಸ್ಥಾನಾಂತರ + i);
	}

	bool can_append(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;
	bool try_append(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	bool try_append(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk);

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ DriveChunkMarker &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ marker == other.marker && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ && ಅಗಲ == other.ಅಗಲ;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ DriveChunkMarker &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (marker != other.marker)
			ಹಿಂತಿರುಗಿ marker < other.marker;
		ಆದರೆ (ಅಗಲ != other.ಅಗಲ)
			ಹಿಂತಿರುಗಿ ಅಗಲ < other.ಅಗಲ;
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ Drive_Chunk_ಅಪವರ್ತ್ಯ
{
ಖಾಸಗಿ:
	mutable pool<DriveChunk> ಅಪವರ್ತ್ಯ_3;
	ಪೂರ್ಣಾಂಕ ಅಗಲ_3;

ಸಾರ್ವಜನಿಕ:
	pool<DriveChunk> ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }

	Drive_Chunk_ಅಪವರ್ತ್ಯ(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ_3; }

	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ;

	bool can_append(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;

	bool try_append(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);


	bool can_append(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;

	bool try_append(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_Chunk_ಅಪವರ್ತ್ಯ &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಅಗಲ_3 == other.ಅಗಲ_3 && ಅಪವರ್ತ್ಯ_3 == other.ಅಪವರ್ತ್ಯ_3;
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_Chunk_ಅಪವರ್ತ್ಯ &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (ಅಪವರ್ತ್ಯ_3.ಗಾತ್ರ_2() < other.ಅಪವರ್ತ್ಯ_3.ಗಾತ್ರ_2())

		ಅಪವರ್ತ್ಯ_3.ವಿಂಗಡಿಸು();
		ಹಿಂತಿರುಗಿ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ DriveChunk
{
ಖಾಸಗಿ:
	DriveType type_ = DriveType::NONE;
	ಒಕ್ಕೂಟ
	{
		ಪೂರ್ಣಾಂಕ none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_ಪೋರ್ಟ್ ಪೋರ್ಟ್_3;
		DriveChunkMarker marker_;
		Drive_Chunk_ಅಪವರ್ತ್ಯ ಅಪವರ್ತ್ಯ_3;
	};

ಸಾರ್ವಜನಿಕ:
	DriveChunk() { ಗಣ_none(); }

	DriveChunk(DriveChunk ಸ್ಥಿರಾಂಕ &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other) { *this = other; }

	DriveChunk(Const ಸ್ಥಿರಾಂಕ &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ಸ್ಥಿರಾಂಕ &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್) { *this = ಪೋರ್ಟ್; }
	DriveChunk(Drive_Chunk_ಪೋರ್ಟ್ &&ಪೋರ್ಟ್) { *this = ಪೋರ್ಟ್; }
	DriveChunk(DriveChunkMarker ಸ್ಥಿರಾಂಕ &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2) { *this = ಅಪವರ್ತ್ಯ_2; }
	DriveChunk(Drive_Chunk_ಅಪವರ್ತ್ಯ &&ಅಪವರ್ತ್ಯ_2) { *this = ಅಪವರ್ತ್ಯ_2; }

	~DriveChunk() { ಗಣ_none(); }

	Drive_ಬಿಟ್ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
	{
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ Drive_ಬಿಟ್();
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_[i];
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_[i];
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3[i];
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_[i];
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3[i];
		}
		log_abort();
	}

	ಶೂನ್ಯ ಗಣ_none(ಪೂರ್ಣಾಂಕ ಅಗಲ = 0)
	{
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				none_ = ಅಗಲ;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				constant_.~Const();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				wire_.~DriveChunkWire();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಪೋರ್ಟ್_3.~Drive_Chunk_ಪೋರ್ಟ್();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಅಪವರ್ತ್ಯ_3.~Drive_Chunk_ಅಪವರ್ತ್ಯ();
				ನಿಲ್ಲಿಸು;
		}
		type_ = DriveType::NONE;
		none_ = ಅಗಲ;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunk ಸ್ಥಿರಾಂಕ &other)
	{
		ಆಯ್ಕೆ (other.type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಗಣ_none(other.none_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				*this = other.constant_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				*this = other.wire_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				*this = other.ಪೋರ್ಟ್_3;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				*this = other.marker_;
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				*this = other.ಅಪವರ್ತ್ಯ_3;
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunk &&other)
	{
		ಆಯ್ಕೆ (other.type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಗಣ_none(other.none_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				*this = std::move(other.wire_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				*this = std::move(other.ಪೋರ್ಟ್_3);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				*this = std::move(other.marker_);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				*this = std::move(other.ಅಪವರ್ತ್ಯ_3);
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Const ಸ್ಥಿರಾಂಕ &constant)
	{
		ಗಣ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Const &&constant)
	{
		ಗಣ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkWire ಸ್ಥಿರಾಂಕ &wire)
	{
		ಗಣ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkWire &&wire)
	{
		ಗಣ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್)
	{
		ಗಣ_none();
		new (&ಪೋರ್ಟ್_3) Drive_Chunk_ಪೋರ್ಟ್(ಪೋರ್ಟ್);
		type_ = DriveType::ಪೋರ್ಟ್_2;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಪೋರ್ಟ್ &&ಪೋರ್ಟ್)
	{
		ಗಣ_none();
		new (&ಪೋರ್ಟ್_3) Drive_Chunk_ಪೋರ್ಟ್(ಪೋರ್ಟ್);
		type_ = DriveType::ಪೋರ್ಟ್_2;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkMarker ಸ್ಥಿರಾಂಕ &marker)
	{
		ಗಣ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkMarker &&marker)
	{
		ಗಣ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2)
	{
		ಗಣ_none(ಅಪವರ್ತ್ಯ_2.ಗಾತ್ರ_2());
		ಆದರೆ (ಅಪವರ್ತ್ಯ_2.ಅಪವರ್ತ್ಯ_2().ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ *this;
		new (&ಅಪವರ್ತ್ಯ_3) Drive_Chunk_ಅಪವರ್ತ್ಯ(ಅಪವರ್ತ್ಯ_2);
		type_ = DriveType::ಅಪವರ್ತ್ಯ;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಅಪವರ್ತ್ಯ &&ಅಪವರ್ತ್ಯ_2)
	{
		ಗಣ_none(ಅಪವರ್ತ್ಯ_2.ಗಾತ್ರ_2());
		ಆದರೆ (ಅಪವರ್ತ್ಯ_2.ಅಪವರ್ತ್ಯ_2().ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ *this;
		new (&ಅಪವರ್ತ್ಯ_3) Drive_Chunk_ಅಪವರ್ತ್ಯ(ಅಪವರ್ತ್ಯ_2);
		type_ = DriveType::ಅಪವರ್ತ್ಯ;
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveChunk &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other)
	{
		ಆಯ್ಕೆ (other.type())
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಗಣ_none(1);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				*this = Const(other.constant());
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				*this = Drive_Chunk_ಪೋರ್ಟ್(other.ಪೋರ್ಟ್());
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				*this = Drive_Chunk_ಅಪವರ್ತ್ಯ(other.ಅಪವರ್ತ್ಯ_2());
				ನಿಲ್ಲಿಸು;
		}
		ಹಿಂತಿರುಗಿ *this;
	}

	bool can_append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ;
	bool try_append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	bool try_append(DriveChunk ಸ್ಥಿರಾಂಕ &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ DriveChunk &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (type_ != other.type_)
			ಹಿಂತಿರುಗಿ false;

		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ true;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_ == other.constant_;
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_ == other.wire_;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3 == other.ಪೋರ್ಟ್_3;
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_ == other.marker_;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3 == other.ಅಪವರ್ತ್ಯ_3;
		}
		log_abort();
	}

	bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ DriveChunk &other) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ !(*this == other);
	}

	bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ DriveChunk &other) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (type_ != other.type_)
			ಹಿಂತಿರುಗಿ type_ < other.type_;
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ false;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_ < other.constant_;
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_ < other.wire_;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3 < other.ಪೋರ್ಟ್_3;
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_ < other.marker_;
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3 < other.ಅಪವರ್ತ್ಯ_3;
		}
		log_abort();
	}

	DriveType type() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_; }

	bool is_none() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::NONE; }
	bool is_constant() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::CONSTANT; }
	bool is_wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::WIRE; }
	bool is_ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::ಪೋರ್ಟ್_2; }
	bool is_marker() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::MARKER; }
	bool is_ಅಪವರ್ತ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ type_ == DriveType::ಅಪವರ್ತ್ಯ; }

	Const &constant() { log_assert(is_constant()); ಹಿಂತಿರುಗಿ constant_; }
	Const ಸ್ಥಿರಾಂಕ &constant() ಸ್ಥಿರಾಂಕ { log_assert(is_constant()); ಹಿಂತಿರುಗಿ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ಹಿಂತಿರುಗಿ wire_; }
	DriveChunkWire ಸ್ಥಿರಾಂಕ &wire() ಸ್ಥಿರಾಂಕ { log_assert(is_wire()); ಹಿಂತಿರುಗಿ wire_; }
	Drive_Chunk_ಪೋರ್ಟ್ &ಪೋರ್ಟ್() { log_assert(is_ಪೋರ್ಟ್()); ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3; }
	Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { log_assert(is_ಪೋರ್ಟ್()); ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ಹಿಂತಿರುಗಿ marker_; }
	DriveChunkMarker ಸ್ಥಿರಾಂಕ &marker() ಸ್ಥಿರಾಂಕ { log_assert(is_marker()); ಹಿಂತಿರುಗಿ marker_; }
	Drive_Chunk_ಅಪವರ್ತ್ಯ &ಅಪವರ್ತ್ಯ_2() { log_assert(is_ಅಪವರ್ತ್ಯ()); ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }
	Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಅಪವರ್ತ್ಯ_2() ಸ್ಥಿರಾಂಕ { log_assert(is_ಅಪವರ್ತ್ಯ()); ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3; }


	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ
	{
		ಆಯ್ಕೆ (type_)
		{
			ಸ್ಥಿತಿ DriveType::NONE:
				ಹಿಂತಿರುಗಿ none_;
			ಸ್ಥಿತಿ DriveType::CONSTANT:
				ಹಿಂತಿರುಗಿ constant_.ಗಾತ್ರ_2();
			ಸ್ಥಿತಿ DriveType::WIRE:
				ಹಿಂತಿರುಗಿ wire_.ಗಾತ್ರ_2();
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3.ಗಾತ್ರ_2();
			ಸ್ಥಿತಿ DriveType::MARKER:
				ಹಿಂತಿರುಗಿ marker_.ಗಾತ್ರ_2();
			ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
				ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3.ಗಾತ್ರ_2();
		}
		log_abort();
	}
};

ಸಾಲೊಳಗೆ Drive_Chunk_ಅಪವರ್ತ್ಯ::Drive_Chunk_ಅಪವರ್ತ್ಯ(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
	: ಅಗಲ_3(1)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಬಿಟ್ : ಬಿಟ್.ಅಪವರ್ತ್ಯ_2())
		ಅಪವರ್ತ್ಯ_3.emplace(ಬಿಟ್);
}

ರಚನೆ DriveSpec
{
ಖಾಸಗಿ:
	ಪೂರ್ಣಾಂಕ ಅಗಲ_3 = 0;
	mutable std::ಸದಿಶ<DriveChunk> chunks_;
	mutable std::ಸದಿಶ<Drive_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_2;
	mutable ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hash_ = 0;
ಸಾರ್ವಜನಿಕ:

	ಸಾಲೊಳಗೆ bool packed() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.ಖಾಲಿಗಳು();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ಸ್ಥಿರಾಂಕ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &chunk) { *this = chunk; }

	DriveSpec(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { *this = ಬಿಟ್; }
	DriveSpec(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { *this = ಬಿಟ್; }
	DriveSpec(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { *this = ಬಿಟ್; }
	DriveSpec(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { *this = ಬಿಟ್; }
	DriveSpec(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { *this = ಬಿಟ್; }

	DriveSpec(std::ಸದಿಶ<DriveChunk> ಸ್ಥಿರಾಂಕ &chunks) : chunks_(chunks) { compute_ಅಗಲ(); }

	DriveSpec(std::ಸದಿಶ<Drive_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &ಬಿಟ್ಟುಗಳು)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			append(ಬಿಟ್);
	}

	DriveSpec(SigSpec ಸ್ಥಿರಾಂಕ &sig)
	{
		// TODO: converting one chunk at a time would be faster
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಬಿಟ್ : sig.ಬಿಟ್ಟುಗಳು())
			append(ಬಿಟ್);
	}

	std::ಸದಿಶ<DriveChunk> ಸ್ಥಿರಾಂಕ &chunks() ಸ್ಥಿರಾಂಕ { pack(); ಹಿಂತಿರುಗಿ chunks_; }
	std::ಸದಿಶ<Drive_ಬಿಟ್> ಸ್ಥಿರಾಂಕ &ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ { unpack(); ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2; }

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ_3; }

	ಶೂನ್ಯ append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);

	ಶೂನ್ಯ append(DriveChunk ಸ್ಥಿರಾಂಕ &chunk);

	ಶೂನ್ಯ pack() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ unpack() ಸ್ಥಿರಾಂಕ;

	Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
	{
		log_assert(ಸೂಚ್ಯಂಕ >= 0 && ಸೂಚ್ಯಂಕ < ಗಾತ್ರ_2());
		unpack();
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2[ಸೂಚ್ಯಂಕ];
	}

	ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ
	{
		log_assert(ಸೂಚ್ಯಂಕ >= 0 && ಸೂಚ್ಯಂಕ < ಗಾತ್ರ_2());
		unpack();
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2[ಸೂಚ್ಯಂಕ];
	}

	ಶೂನ್ಯ clear()
	{
		chunks_.clear();
		ಬಿಟ್ಟುಗಳು_2.clear();
		ಅಗಲ_3 = 0;
	}

	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(DriveChunk ಸ್ಥಿರಾಂಕ &chunk)
	{
		chunks_.clear();
		ಬಿಟ್ಟುಗಳು_2.clear();
		append(chunk);
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk) { ಹಿಂತಿರುಗಿ *this = DriveChunk(chunk); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk) { ಹಿಂತಿರುಗಿ *this = DriveChunk(chunk); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk) { ಹಿಂತಿರುಗಿ *this = DriveChunk(chunk); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &chunk) { ಹಿಂತಿರುಗಿ *this = DriveChunk(chunk); }

	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
	{
		chunks_.clear();
		ಬಿಟ್ಟುಗಳು_2.clear();
		append(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ *this;
	}

	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { ಹಿಂತಿರುಗಿ *this = Drive_ಬಿಟ್(ಬಿಟ್); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { ಹಿಂತಿರುಗಿ *this = Drive_ಬಿಟ್(ಬಿಟ್); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { ಹಿಂತಿರುಗಿ *this = Drive_ಬಿಟ್(ಬಿಟ್); }
	DriveSpec &ಕ್ರಿಯಾಕಾರಕ=(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { ಹಿಂತಿರುಗಿ *this = Drive_ಬಿಟ್(ಬಿಟ್); }

	ಶೂನ್ಯ updhash() ಸ್ಥಿರಾಂಕ {
		ಆದರೆ (hash_ != 0)
			ಹಿಂತಿರುಗಿ;
		pack();
		hash_ = ಸತತ_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;

	bool ಕ್ರಿಯಾಕಾರಕ==(DriveSpec ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ {
		updhash();
		other.updhash();
		ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2() || hash_ != other.hash_)
			ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ chunks() == other.chunks();
	}

ಖಾಸಗಿ:
	ಶೂನ್ಯ compute_ಅಗಲ();
};



ರಚನೆ DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.ಸಿದ್ಧತೆ(); }
	DriverMap(Design *design) { celltypes.ಸಿದ್ಧತೆ(design); }

ಖಾಸಗಿ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ರಚನೆ Drive_ಬಿಟ್_Id
	{
		ಪೂರ್ಣಾಂಕ id = -1;

		Drive_ಬಿಟ್_Id() {};

		Drive_ಬಿಟ್_Id(ಪೂರ್ಣಾಂಕ id) : id(id) { }

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Id &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ id == other.id; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Id &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ id != other.id; }
		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ Drive_ಬಿಟ್_Id &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ರಚನೆ Drive_ಬಿಟ್_ನಕ್ಷೆ {
		dict<Drive_ಬಿಟ್_Id, Drive_ಬಿಟ್_Id> ಮೊದಲ_ಅಂಚುಗಳು;
		dict<Drive_ಬಿಟ್_Id, Drive_ಬಿಟ್_Id> ಎರಡನೇ_ಅಂಚುಗಳು;
		dict<Drive_ಬಿಟ್_Id, pool<Drive_ಬಿಟ್_Id>> ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು;

		ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(Drive_ಬಿಟ್_Id src, Drive_ಬಿಟ್_Id dst);
		Drive_ಬಿಟ್_Id ತೆಗೆ_ಅಂಚು(Drive_ಬಿಟ್_Id src);
		ಶೂನ್ಯ clear(Drive_ಬಿಟ್_Id src);
		bool ಒಳಗೊಂಡಿದೆ(Drive_ಬಿಟ್_Id src);
		ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(Drive_ಬಿಟ್_Id src);

		Drive_ಬಿಟ್_Id at(Drive_ಬಿಟ್_Id src, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_ಬಿಟ್_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_ಬಿಟ್_Id> ಪೋರ್ಟ್_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_ಬಿಟ್_Id, Drive_ಬಿಟ್> drive_ಬಿಟ್ಟುಗಳು;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_ಬಿಟ್_Id, Drive_ಬಿಟ್> isolated_drive_ಬಿಟ್ಟುಗಳು;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ = 1 + (ಪೂರ್ಣಾಂಕ)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_ಬಿಟ್_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_ಬಿಟ್_ನಕ್ಷೆ connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_ಬಿಟ್_ನಕ್ಷೆ connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_ಬಿಟ್_ನಕ್ಷೆ connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_ಬಿಟ್_Id> oriented_present;


	ಎಣಿಕೆ class ಬಿಟ್_ಬಹುಲಕ {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	ಬಿಟ್_ಬಹುಲಕ ಬಿಟ್_ಬಹುಲಕ_2(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	Drive_ಬಿಟ್_Id id_from_drive_ಬಿಟ್(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);
	Drive_ಬಿಟ್ drive_ಬಿಟ್_from_id(Drive_ಬಿಟ್_Id id);

	ಶೂನ್ಯ connect_directed_ವಿಲೀನ(Drive_ಬಿಟ್_Id driven_id, Drive_ಬಿಟ್_Id driver_id);
	ಶೂನ್ಯ connect_directed_ಬಫರ್(Drive_ಬಿಟ್_Id driven_id, Drive_ಬಿಟ್_Id driver_id);
	ಶೂನ್ಯ connect_undirected(Drive_ಬಿಟ್_Id a_id, Drive_ಬಿಟ್_Id b_id);

ಸಾರ್ವಜನಿಕ:

	ಶೂನ್ಯ ಕೂಡಿಸು(Module *module);

	// Add a single bit connection to the driver map.
	ಶೂನ್ಯ ಕೂಡಿಸು(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &a, Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &b);

	ಮಾದರಿ<typename T>
	ಸ್ಥಿರ constexpr bool is_sig_type() {
		ಹಿಂತಿರುಗಿ
			std::is_same<T, SigSpec>::ಮೌಲ್ಯ ||
			std::is_same<T, SigChunk>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveSpec>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveChunk>::ಮೌಲ್ಯ ||
			std::is_same<T, Drive_Chunk_ಪೋರ್ಟ್>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveChunkWire>::ಮೌಲ್ಯ ||
			std::is_same<T, Const>::ಮೌಲ್ಯ;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	ಮಾದರಿ<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	ಕೂಡಿಸು(T ಸ್ಥಿರಾಂಕ &a, U ಸ್ಥಿರಾಂಕ &b)
	{
		log_assert(a.ಗಾತ್ರ_2() == b.ಗಾತ್ರ_2());
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != Get_ಗಾತ್ರ(a); ++i)
			ಕೂಡಿಸು(Drive_ಬಿಟ್(a[i]), Drive_ಬಿಟ್(b[i]));
	}


	// Specialized version that avoids unpacking
	ಶೂನ್ಯ ಕೂಡಿಸು(SigSpec ಸ್ಥಿರಾಂಕ &a, SigSpec ಸ್ಥಿರಾಂಕ &b);

ಖಾಸಗಿ:
	ಶೂನ್ಯ ಕೂಡಿಸು_ಪೋರ್ಟ್(Cell *cell, IdString ಪೋರ್ಟ್, SigSpec ಸ್ಥಿರಾಂಕ &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_ಬಿಟ್_Id> orient_undirected_ನೋಡಿದ;
	pool<Drive_ಬಿಟ್_Id> orient_undirected_drivers;
	dict<Drive_ಬಿಟ್_Id, ಪೂರ್ಣಾಂಕ> orient_undirected_ದೂರ;

	ಶೂನ್ಯ orient_undirected(Drive_ಬಿಟ್_Id id);

ಸಾರ್ವಜನಿಕ:
	Drive_ಬಿಟ್ ಕ್ರಿಯಾಕಾರಕ()(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್);

	DriveSpec ಕ್ರಿಯಾಕಾರಕ()(DriveSpec spec);

ಖಾಸಗಿ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ಹಿಂತಿರುಗಿ wire->has_attribute(ID(keep));
	}
};

ಸಾಲೊಳಗೆ Hasher Drive_ಬಿಟ್_Wire::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(wire->name);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_ಬಿಟ್_ಪೋರ್ಟ್::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(cell->name);
	h.eat(ಪೋರ್ಟ್);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_ಬಿಟ್_Marker::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(marker);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(ಅಪವರ್ತ್ಯ_3);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_ಬಿಟ್::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	ಆಯ್ಕೆ (type_) {
	ಸ್ಥಿತಿ DriveType::NONE:
		h.eat(0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::CONSTANT:
		h.eat(constant_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::WIRE:
		h.eat(wire_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
		h.eat(ಪೋರ್ಟ್_3);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::MARKER:
		h.eat(marker_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
		h.eat(ಅಪವರ್ತ್ಯ_3);
		ನಿಲ್ಲಿಸು;
	}
	h.eat(type_);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher DriveChunkWire::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(wire->name);
	h.eat(ಅಗಲ);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_Chunk_ಪೋರ್ಟ್::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(cell->name);
	h.eat(ಪೋರ್ಟ್);
	h.eat(ಅಗಲ);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher DriveChunkMarker::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(marker);
	h.eat(ಅಗಲ);
	h.eat(ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher Drive_Chunk_ಅಪವರ್ತ್ಯ::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(ಅಗಲ_3);
	h.eat(ಅಪವರ್ತ್ಯ_3);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher DriveChunk::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	ಆಯ್ಕೆ (type_) {
	ಸ್ಥಿತಿ DriveType::NONE:
		h.eat(0);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::CONSTANT:
		h.eat(constant_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::WIRE:
		h.eat(wire_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
		h.eat(ಪೋರ್ಟ್_3);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::MARKER:
		h.eat(marker_);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
		h.eat(ಅಪವರ್ತ್ಯ_3);
		ನಿಲ್ಲಿಸು;
	}
	h.eat(type_);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher DriveSpec::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	updhash();
	h.eat(hash_);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ Hasher DriverMap::Drive_ಬಿಟ್_Id::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(id);
	ಹಿಂತಿರುಗಿ h;
}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
