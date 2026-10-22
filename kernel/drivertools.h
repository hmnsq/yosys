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

#यदि_अपरिभाषितम् DRIVERTOOLS_H
#परिभाषय DRIVERTOOLS_H

#अन्तर्भाव <string>
#अन्तर्भाव <type_traits>

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

संरचना DriveBit;

संरचना DriveChunkWire;
संरचना Drive_Chunk_पत्तनम्;
संरचना DriveChunkMarker;
संरचना DriveChunk;

संरचना DriveSpec;

std::string log_signal(DriveChunkWire ध्रुव &chunk);
std::string log_signal(Drive_Chunk_पत्तनम् ध्रुव &chunk);
std::string log_signal(DriveChunkMarker ध्रुव &chunk);
std::string log_signal(DriveChunk ध्रुव &chunk);
std::string log_signal(DriveSpec ध्रुव &chunk);

गणना class DriveType : अचिह्नित वर्ण
{
	NONE,
	CONSTANT,
	WIRE,
	पत्तनम्_2,
	गुणजः,
	MARKER,
};

संरचना DriveBitWire
{
	Wire *wire;
	पूर्णाङ्क विस्थापनम्;

	DriveBitWire(Wire *wire, पूर्णाङ्क विस्थापनम्) : wire(wire), विस्थापनम्(विस्थापनम्) {}

	bool संक्रियकः==(ध्रुव DriveBitWire &other) ध्रुव
	{
		प्रत्यागम wire == other.wire && विस्थापनम् == other.विस्थापनम्;
	}

	bool संक्रियकः<(ध्रुव DriveBitWire &other) ध्रुव
	{
		यदि (wire != other.wire)
			प्रत्यागम wire->name < other.wire->name;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;


	संक्रियकः SigBit() ध्रुव
	{
		प्रत्यागम SigBit(wire, विस्थापनम्);
	}
};

संरचना Drive_Bit_पत्तनम्
{
	Cell *cell;
	IdString पत्तनम्;
	पूर्णाङ्क विस्थापनम्;

	Drive_Bit_पत्तनम्(Cell *cell, IdString पत्तनम्, पूर्णाङ्क विस्थापनम्) : cell(cell), पत्तनम्(पत्तनम्), विस्थापनम्(विस्थापनम्) {}

	bool संक्रियकः==(ध्रुव Drive_Bit_पत्तनम् &other) ध्रुव
	{
		प्रत्यागम cell == other.cell && पत्तनम् == other.पत्तनम् && विस्थापनम् == other.विस्थापनम्;
	}

	bool संक्रियकः<(ध्रुव Drive_Bit_पत्तनम् &other) ध्रुव
	{
		यदि (cell != other.cell)
			प्रत्यागम cell->name < other.cell->name;
		यदि (पत्तनम् != other.पत्तनम्)
			प्रत्यागम पत्तनम् < other.पत्तनम्;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

};


संरचना DriveBitMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क विस्थापनम्;

	DriveBitMarker(पूर्णाङ्क marker, पूर्णाङ्क विस्थापनम्) : marker(marker), विस्थापनम्(विस्थापनम्) {}

	bool संक्रियकः==(ध्रुव DriveBitMarker &other) ध्रुव
	{
		प्रत्यागम marker == other.marker && विस्थापनम् == other.विस्थापनम्;
	}

	bool संक्रियकः<(ध्रुव DriveBitMarker &other) ध्रुव
	{
		यदि (marker != other.marker)
			प्रत्यागम marker < other.marker;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

};

संरचना Drive_Bit_गुणजः
{
गुप्त:
	pool<DriveBit> गुणजः_3;

सार्वजनिक:
	Drive_Bit_गुणजः();
	Drive_Bit_गुणजः(DriveBit ध्रुव &single);

	pool<DriveBit> ध्रुव &गुणजः_2() ध्रुव { प्रत्यागम गुणजः_3; }

	शून्य merge(Drive_Bit_गुणजः ध्रुव &other)
	{
		प्रति (DriveBit ध्रुव &single : other.गुणजः_3)
			merge(single);
	}

	शून्य merge(Drive_Bit_गुणजः &&other)
	{
		प्रति (DriveBit &single : other.गुणजः_3)
			merge(std::move(single));
	}

	शून्य merge(DriveBit ध्रुव &single);
	शून्य merge(DriveBit &&single);

	bool संक्रियकः==(ध्रुव Drive_Bit_गुणजः &other) ध्रुव
	{
		प्रत्यागम गुणजः_3 == other.गुणजः_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना DriveBit
{
गुप्त:
	DriveType type_ = DriveType::NONE;
	सङ्घ
	{
		State constant_;
		DriveBitWire wire_;
		Drive_Bit_पत्तनम् पत्तनम्_3;
		DriveBitMarker marker_;
		Drive_Bit_गुणजः गुणजः_3;
	};
सार्वजनिक:
	DriveBit()  {}

	DriveBit(SigBit ध्रुव &bit);

	DriveBit(DriveBit ध्रुव &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ध्रुव &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(Drive_Bit_पत्तनम् ध्रुव &पत्तनम्) { *this = पत्तनम्; }
	DriveBit(Drive_Bit_पत्तनम् &&पत्तनम्) { *this = पत्तनम्; }
	DriveBit(DriveBitMarker ध्रुव &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(Drive_Bit_गुणजः ध्रुव &गुणजः_2) { *this = गुणजः_2; }
	DriveBit(Drive_Bit_गुणजः &&गुणजः_2) { *this = गुणजः_2; }

	~DriveBit() { समुच्चयः_none(); }

	शून्य समुच्चयः_none()
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				भङ्ग;
			पक्ष DriveType::WIRE:
				wire_.~DriveBitWire();
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				पत्तनम्_3.~Drive_Bit_पत्तनम्();
				भङ्ग;
			पक्ष DriveType::MARKER:
				marker_.~DriveBitMarker();
				भङ्ग;
			पक्ष DriveType::गुणजः:
				गुणजः_3.~Drive_Bit_गुणजः();
				भङ्ग;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &संक्रियकः=(DriveBit ध्रुव &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				समुच्चयः_none();
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				*this = other.पत्तनम्_3;
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				भङ्ग;
			पक्ष DriveType::गुणजः:
				*this = other.गुणजः_3;
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(DriveBit &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				समुच्चयः_none();
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				*this = std::move(other.पत्तनम्_3);
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				भङ्ग;
			पक्ष DriveType::गुणजः:
				*this = std::move(other.गुणजः_3);
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(State constant)
	{
		समुच्चयः_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(DriveBitWire ध्रुव &wire)
	{
		समुच्चयः_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(DriveBitWire &&wire)
	{
		समुच्चयः_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(Drive_Bit_पत्तनम् ध्रुव &पत्तनम्)
	{
		समुच्चयः_none();
		new (&पत्तनम्_3) Drive_Bit_पत्तनम्(पत्तनम्);
		type_ = DriveType::पत्तनम्_2;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(Drive_Bit_पत्तनम् &&पत्तनम्)
	{
		समुच्चयः_none();
		new (&पत्तनम्_3) Drive_Bit_पत्तनम्(पत्तनम्);
		type_ = DriveType::पत्तनम्_2;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(DriveBitMarker ध्रुव &marker)
	{
		समुच्चयः_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(DriveBitMarker &&marker)
	{
		समुच्चयः_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(Drive_Bit_गुणजः ध्रुव &गुणजः_2)
	{
		समुच्चयः_none();
		यदि (गुणजः_2.गुणजः_2().रिक्तानि())
			प्रत्यागम *this;
		new (&गुणजः_3) Drive_Bit_गुणजः(गुणजः_2);
		type_ = DriveType::गुणजः;
		प्रत्यागम *this;
	}

	DriveBit &संक्रियकः=(Drive_Bit_गुणजः &&गुणजः_2)
	{
		समुच्चयः_none();
		यदि (गुणजः_2.गुणजः_2().रिक्तानि())
			प्रत्यागम *this;
		new (&गुणजः_3) Drive_Bit_गुणजः(गुणजः_2);
		type_ = DriveType::गुणजः;
		प्रत्यागम *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool संक्रियकः==(ध्रुव DriveBit &other) ध्रुव
	{
		यदि (type_ != other.type_)
			प्रत्यागम false;

		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम true;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_ == other.constant_;
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_ == other.wire_;
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3 == other.पत्तनम्_3;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ == other.marker_;
			पक्ष DriveType::गुणजः:
				प्रत्यागम गुणजः_3 == other.गुणजः_3;
		}
		log_abort();
	}

	bool संक्रियकः!=(ध्रुव DriveBit &other) ध्रुव
	{
		प्रत्यागम !(*this == other);
	}

	bool संक्रियकः<(ध्रुव DriveBit &other) ध्रुव
	{
		यदि (type_ != other.type_)
			प्रत्यागम type_ < other.type_;
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम false;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_ < other.constant_;
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_ < other.wire_;
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3 < other.पत्तनम्_3;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ < other.marker_;
			पक्ष DriveType::गुणजः:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ध्रुव { प्रत्यागम type_; }

	bool is_none() ध्रुव { प्रत्यागम type_ == DriveType::NONE; }
	bool is_constant() ध्रुव { प्रत्यागम type_ == DriveType::CONSTANT; }
	bool is_wire() ध्रुव { प्रत्यागम type_ == DriveType::WIRE; }
	bool is_पत्तनम्() ध्रुव { प्रत्यागम type_ == DriveType::पत्तनम्_2; }
	bool is_marker() ध्रुव { प्रत्यागम type_ == DriveType::MARKER; }
	bool is_गुणजः() ध्रुव { प्रत्यागम type_ == DriveType::गुणजः; }

	State &constant() { log_assert(is_constant()); प्रत्यागम constant_; }
	State ध्रुव &constant() ध्रुव { log_assert(is_constant()); प्रत्यागम constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveBitWire ध्रुव &wire() ध्रुव { log_assert(is_wire()); प्रत्यागम wire_; }
	Drive_Bit_पत्तनम् &पत्तनम्() { log_assert(is_पत्तनम्()); प्रत्यागम पत्तनम्_3; }
	Drive_Bit_पत्तनम् ध्रुव &पत्तनम्() ध्रुव { log_assert(is_पत्तनम्()); प्रत्यागम पत्तनम्_3; }
	DriveBitMarker &marker() { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveBitMarker ध्रुव &marker() ध्रुव { log_assert(is_marker()); प्रत्यागम marker_; }
	Drive_Bit_गुणजः &गुणजः_2() { log_assert(is_गुणजः()); प्रत्यागम गुणजः_3; }
	Drive_Bit_गुणजः ध्रुव &गुणजः_2() ध्रुव { log_assert(is_गुणजः()); प्रत्यागम गुणजः_3; }

	शून्य merge(DriveBit ध्रुव &other);

};

अन्तःस्थ Drive_Bit_गुणजः::Drive_Bit_गुणजः() {}
अन्तःस्थ Drive_Bit_गुणजः::Drive_Bit_गुणजः(DriveBit ध्रुव &single)
{
	गुणजः_3.emplace(single);
}

संरचना DriveChunkWire
{
	Wire *wire;
	पूर्णाङ्क विस्थापनम्;
	पूर्णाङ्क विस्तारः;

	DriveChunkWire(Wire *wire, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः) : wire(wire), विस्थापनम्(विस्थापनम्), विस्तारः(विस्तारः) {}
	DriveChunkWire(DriveBitWire ध्रुव &bit) : wire(bit.wire), विस्थापनम्(bit.विस्थापनम्), विस्तारः(1) {}

	पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः; }

	DriveBitWire संक्रियकः[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < विस्तारः);
		प्रत्यागम DriveBitWire(wire, विस्थापनम् + i);
	}

	bool can_append(DriveBitWire ध्रुव &bit) ध्रुव;
	bool try_append(DriveBitWire ध्रुव &bit);
	bool try_append(DriveChunkWire ध्रुव &chunk);

	// Whether this chunk is a whole wire
	bool is_सम्पूर्णम्() ध्रुव { प्रत्यागम विस्थापनम् == 0 && विस्तारः == wire->विस्तारः; }

	bool संक्रियकः==(ध्रुव DriveChunkWire &other) ध्रुव
	{
		प्रत्यागम wire == other.wire && विस्थापनम् == other.विस्थापनम् && विस्तारः == other.विस्तारः;
	}

	bool संक्रियकः<(ध्रुव DriveChunkWire &other) ध्रुव
	{
		यदि (wire != other.wire)
			प्रत्यागम wire->name < other.wire->name;
		यदि (विस्तारः != other.विस्तारः)
			प्रत्यागम विस्तारः < other.विस्तारः;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	explicit संक्रियकः SigChunk() ध्रुव
	{
		प्रत्यागम SigChunk(wire, विस्थापनम्, विस्तारः);
	}
};

संरचना Drive_Chunk_पत्तनम्
{
	Cell *cell;
	IdString पत्तनम्;
	पूर्णाङ्क विस्थापनम्;
	पूर्णाङ्क विस्तारः;

	Drive_Chunk_पत्तनम्(Cell *cell, IdString पत्तनम्, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः) :
		cell(cell), पत्तनम्(पत्तनम्), विस्थापनम्(विस्थापनम्), विस्तारः(विस्तारः) { }
	Drive_Chunk_पत्तनम्(Cell *cell, IdString पत्तनम्) :
		cell(cell), पत्तनम्(पत्तनम्), विस्थापनम्(0), विस्तारः(Get_आकार(cell->connections().at(पत्तनम्))) { }
	Drive_Chunk_पत्तनम्(Cell *cell, std::pair<IdString, SigSpec> ध्रुव &conn) :
		cell(cell), पत्तनम्(conn.first), विस्थापनम्(0), विस्तारः(Get_आकार(conn.second)) { }
	Drive_Chunk_पत्तनम्(Drive_Bit_पत्तनम् ध्रुव &bit) :
		cell(bit.cell), पत्तनम्(bit.पत्तनम्), विस्थापनम्(bit.विस्थापनम्), विस्तारः(1) { }

	पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः; }

	Drive_Bit_पत्तनम् संक्रियकः[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < विस्तारः);
		प्रत्यागम Drive_Bit_पत्तनम्(cell, पत्तनम्, विस्थापनम् + i);
	}

	bool can_append(Drive_Bit_पत्तनम् ध्रुव &bit) ध्रुव;
	bool try_append(Drive_Bit_पत्तनम् ध्रुव &bit);
	bool try_append(Drive_Chunk_पत्तनम् ध्रुव &chunk);

	// Whether this chunk is a whole port
	bool is_सम्पूर्णम्() ध्रुव
	{
		प्रत्यागम विस्थापनम् == 0 && विस्तारः == cell->connections().at(पत्तनम्).आकार();
	}

	bool संक्रियकः==(ध्रुव Drive_Chunk_पत्तनम् &other) ध्रुव
	{
		प्रत्यागम cell == other.cell && पत्तनम् == other.पत्तनम् && विस्थापनम् == other.विस्थापनम् && विस्तारः == other.विस्तारः;
	}

	bool संक्रियकः<(ध्रुव Drive_Chunk_पत्तनम् &other) ध्रुव
	{
		यदि (cell != other.cell)
			प्रत्यागम cell->name < other.cell->name;
		यदि (पत्तनम् != other.पत्तनम्)
			प्रत्यागम पत्तनम् < other.पत्तनम्;
		यदि (विस्तारः != other.विस्तारः)
			प्रत्यागम विस्तारः < other.विस्तारः;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};


संरचना DriveChunkMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क विस्थापनम्;
	पूर्णाङ्क विस्तारः;

	DriveChunkMarker(पूर्णाङ्क marker, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः) :
		marker(marker), विस्थापनम्(विस्थापनम्), विस्तारः(विस्तारः) {}
	DriveChunkMarker(DriveBitMarker ध्रुव &bit) :
		marker(bit.marker), विस्थापनम्(bit.विस्थापनम्), विस्तारः(1) {}

	पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः; }

	DriveBitMarker संक्रियकः[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < विस्तारः);
		प्रत्यागम DriveBitMarker(marker, विस्थापनम् + i);
	}

	bool can_append(DriveBitMarker ध्रुव &bit) ध्रुव;
	bool try_append(DriveBitMarker ध्रुव &bit);
	bool try_append(DriveChunkMarker ध्रुव &chunk);

	bool संक्रियकः==(ध्रुव DriveChunkMarker &other) ध्रुव
	{
		प्रत्यागम marker == other.marker && विस्थापनम् == other.विस्थापनम् && विस्तारः == other.विस्तारः;
	}

	bool संक्रियकः<(ध्रुव DriveChunkMarker &other) ध्रुव
	{
		यदि (marker != other.marker)
			प्रत्यागम marker < other.marker;
		यदि (विस्तारः != other.विस्तारः)
			प्रत्यागम विस्तारः < other.विस्तारः;
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना Drive_Chunk_गुणजः
{
गुप्त:
	mutable pool<DriveChunk> गुणजः_3;
	पूर्णाङ्क विस्तारः_3;

सार्वजनिक:
	pool<DriveChunk> ध्रुव &गुणजः_2() ध्रुव { प्रत्यागम गुणजः_3; }

	Drive_Chunk_गुणजः(Drive_Bit_गुणजः ध्रुव &bit);

	पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः_3; }

	Drive_Bit_गुणजः संक्रियकः[](पूर्णाङ्क i) ध्रुव;

	bool can_append(Drive_Bit_गुणजः ध्रुव &bit) ध्रुव;

	bool try_append(Drive_Bit_गुणजः ध्रुव &bit);


	bool can_append(Drive_Chunk_गुणजः ध्रुव &bit) ध्रुव;

	bool try_append(Drive_Chunk_गुणजः ध्रुव &bit);

	bool संक्रियकः==(ध्रुव Drive_Chunk_गुणजः &other) ध्रुव
	{
		प्रत्यागम विस्तारः_3 == other.विस्तारः_3 && गुणजः_3 == other.गुणजः_3;
	}

	bool संक्रियकः<(ध्रुव Drive_Chunk_गुणजः &other) ध्रुव
	{
		यदि (गुणजः_3.आकार() < other.गुणजः_3.आकार())

		गुणजः_3.क्रमय();
		प्रत्यागम false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना DriveChunk
{
गुप्त:
	DriveType type_ = DriveType::NONE;
	सङ्घ
	{
		पूर्णाङ्क none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_पत्तनम् पत्तनम्_3;
		DriveChunkMarker marker_;
		Drive_Chunk_गुणजः गुणजः_3;
	};

सार्वजनिक:
	DriveChunk() { समुच्चयः_none(); }

	DriveChunk(DriveChunk ध्रुव &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ध्रुव &other) { *this = other; }

	DriveChunk(Const ध्रुव &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ध्रुव &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_पत्तनम् ध्रुव &पत्तनम्) { *this = पत्तनम्; }
	DriveChunk(Drive_Chunk_पत्तनम् &&पत्तनम्) { *this = पत्तनम्; }
	DriveChunk(DriveChunkMarker ध्रुव &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_गुणजः ध्रुव &गुणजः_2) { *this = गुणजः_2; }
	DriveChunk(Drive_Chunk_गुणजः &&गुणजः_2) { *this = गुणजः_2; }

	~DriveChunk() { समुच्चयः_none(); }

	DriveBit संक्रियकः[](पूर्णाङ्क i) ध्रुव
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम DriveBit();
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_[i];
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_[i];
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3[i];
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_[i];
			पक्ष DriveType::गुणजः:
				प्रत्यागम गुणजः_3[i];
		}
		log_abort();
	}

	शून्य समुच्चयः_none(पूर्णाङ्क विस्तारः = 0)
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				none_ = विस्तारः;
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				constant_.~Const();
				भङ्ग;
			पक्ष DriveType::WIRE:
				wire_.~DriveChunkWire();
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				पत्तनम्_3.~Drive_Chunk_पत्तनम्();
				भङ्ग;
			पक्ष DriveType::MARKER:
				marker_.~DriveChunkMarker();
				भङ्ग;
			पक्ष DriveType::गुणजः:
				गुणजः_3.~Drive_Chunk_गुणजः();
				भङ्ग;
		}
		type_ = DriveType::NONE;
		none_ = विस्तारः;
	}

	DriveChunk &संक्रियकः=(DriveChunk ध्रुव &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				समुच्चयः_none(other.none_);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				*this = other.पत्तनम्_3;
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				भङ्ग;
			पक्ष DriveType::गुणजः:
				*this = other.गुणजः_3;
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveChunk &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				समुच्चयः_none(other.none_);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				*this = std::move(other.पत्तनम्_3);
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				भङ्ग;
			पक्ष DriveType::गुणजः:
				*this = std::move(other.गुणजः_3);
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Const ध्रुव &constant)
	{
		समुच्चयः_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Const &&constant)
	{
		समुच्चयः_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveChunkWire ध्रुव &wire)
	{
		समुच्चयः_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveChunkWire &&wire)
	{
		समुच्चयः_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Drive_Chunk_पत्तनम् ध्रुव &पत्तनम्)
	{
		समुच्चयः_none();
		new (&पत्तनम्_3) Drive_Chunk_पत्तनम्(पत्तनम्);
		type_ = DriveType::पत्तनम्_2;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Drive_Chunk_पत्तनम् &&पत्तनम्)
	{
		समुच्चयः_none();
		new (&पत्तनम्_3) Drive_Chunk_पत्तनम्(पत्तनम्);
		type_ = DriveType::पत्तनम्_2;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveChunkMarker ध्रुव &marker)
	{
		समुच्चयः_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveChunkMarker &&marker)
	{
		समुच्चयः_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Drive_Chunk_गुणजः ध्रुव &गुणजः_2)
	{
		समुच्चयः_none(गुणजः_2.आकार());
		यदि (गुणजः_2.गुणजः_2().रिक्तानि())
			प्रत्यागम *this;
		new (&गुणजः_3) Drive_Chunk_गुणजः(गुणजः_2);
		type_ = DriveType::गुणजः;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(Drive_Chunk_गुणजः &&गुणजः_2)
	{
		समुच्चयः_none(गुणजः_2.आकार());
		यदि (गुणजः_2.गुणजः_2().रिक्तानि())
			प्रत्यागम *this;
		new (&गुणजः_3) Drive_Chunk_गुणजः(गुणजः_2);
		type_ = DriveType::गुणजः;
		प्रत्यागम *this;
	}

	DriveChunk &संक्रियकः=(DriveBit ध्रुव &other)
	{
		विकल्प (other.type())
		{
			पक्ष DriveType::NONE:
				समुच्चयः_none(1);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = Const(other.constant());
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				भङ्ग;
			पक्ष DriveType::पत्तनम्_2:
				*this = Drive_Chunk_पत्तनम्(other.पत्तनम्());
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				भङ्ग;
			पक्ष DriveType::गुणजः:
				*this = Drive_Chunk_गुणजः(other.गुणजः_2());
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	bool can_append(DriveBit ध्रुव &bit) ध्रुव;
	bool try_append(DriveBit ध्रुव &bit);
	bool try_append(DriveChunk ध्रुव &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool संक्रियकः==(ध्रुव DriveChunk &other) ध्रुव
	{
		यदि (type_ != other.type_)
			प्रत्यागम false;

		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम true;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_ == other.constant_;
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_ == other.wire_;
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3 == other.पत्तनम्_3;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ == other.marker_;
			पक्ष DriveType::गुणजः:
				प्रत्यागम गुणजः_3 == other.गुणजः_3;
		}
		log_abort();
	}

	bool संक्रियकः!=(ध्रुव DriveChunk &other) ध्रुव
	{
		प्रत्यागम !(*this == other);
	}

	bool संक्रियकः<(ध्रुव DriveChunk &other) ध्रुव
	{
		यदि (type_ != other.type_)
			प्रत्यागम type_ < other.type_;
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम false;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_ < other.constant_;
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_ < other.wire_;
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3 < other.पत्तनम्_3;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ < other.marker_;
			पक्ष DriveType::गुणजः:
				प्रत्यागम गुणजः_3 < other.गुणजः_3;
		}
		log_abort();
	}

	DriveType type() ध्रुव { प्रत्यागम type_; }

	bool is_none() ध्रुव { प्रत्यागम type_ == DriveType::NONE; }
	bool is_constant() ध्रुव { प्रत्यागम type_ == DriveType::CONSTANT; }
	bool is_wire() ध्रुव { प्रत्यागम type_ == DriveType::WIRE; }
	bool is_पत्तनम्() ध्रुव { प्रत्यागम type_ == DriveType::पत्तनम्_2; }
	bool is_marker() ध्रुव { प्रत्यागम type_ == DriveType::MARKER; }
	bool is_गुणजः() ध्रुव { प्रत्यागम type_ == DriveType::गुणजः; }

	Const &constant() { log_assert(is_constant()); प्रत्यागम constant_; }
	Const ध्रुव &constant() ध्रुव { log_assert(is_constant()); प्रत्यागम constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveChunkWire ध्रुव &wire() ध्रुव { log_assert(is_wire()); प्रत्यागम wire_; }
	Drive_Chunk_पत्तनम् &पत्तनम्() { log_assert(is_पत्तनम्()); प्रत्यागम पत्तनम्_3; }
	Drive_Chunk_पत्तनम् ध्रुव &पत्तनम्() ध्रुव { log_assert(is_पत्तनम्()); प्रत्यागम पत्तनम्_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveChunkMarker ध्रुव &marker() ध्रुव { log_assert(is_marker()); प्रत्यागम marker_; }
	Drive_Chunk_गुणजः &गुणजः_2() { log_assert(is_गुणजः()); प्रत्यागम गुणजः_3; }
	Drive_Chunk_गुणजः ध्रुव &गुणजः_2() ध्रुव { log_assert(is_गुणजः()); प्रत्यागम गुणजः_3; }


	पूर्णाङ्क आकार() ध्रुव
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम none_;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_.आकार();
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_.आकार();
			पक्ष DriveType::पत्तनम्_2:
				प्रत्यागम पत्तनम्_3.आकार();
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_.आकार();
			पक्ष DriveType::गुणजः:
				प्रत्यागम गुणजः_3.आकार();
		}
		log_abort();
	}
};

अन्तःस्थ Drive_Chunk_गुणजः::Drive_Chunk_गुणजः(Drive_Bit_गुणजः ध्रुव &bit)
	: विस्तारः_3(1)
{
	प्रति (स्वचालित ध्रुव &bit : bit.गुणजः_2())
		गुणजः_3.emplace(bit);
}

संरचना DriveSpec
{
गुप्त:
	पूर्णाङ्क विस्तारः_3 = 0;
	mutable std::सदिशम्<DriveChunk> chunks_;
	mutable std::सदिशम्<DriveBit> द्वयङ्काः_2;
	mutable अचिह्नित पूर्णाङ्क hash_ = 0;
सार्वजनिक:

	अन्तःस्थ bool packed() ध्रुव {
		प्रत्यागम द्वयङ्काः_2.रिक्तानि();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ध्रुव &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_पत्तनम् ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ध्रुव &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_गुणजः ध्रुव &chunk) { *this = chunk; }

	DriveSpec(DriveBit ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitWire ध्रुव &bit) { *this = bit; }
	DriveSpec(Drive_Bit_पत्तनम् ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ध्रुव &bit) { *this = bit; }
	DriveSpec(Drive_Bit_गुणजः ध्रुव &bit) { *this = bit; }

	DriveSpec(std::सदिशम्<DriveChunk> ध्रुव &chunks) : chunks_(chunks) { compute_विस्तारः(); }

	DriveSpec(std::सदिशम्<DriveBit> ध्रुव &द्वयङ्काः)
	{
		प्रति (स्वचालित ध्रुव &bit : द्वयङ्काः)
			append(bit);
	}

	DriveSpec(SigSpec ध्रुव &sig)
	{
		// TODO: converting one chunk at a time would be faster
		प्रति (स्वचालित ध्रुव &bit : sig.द्वयङ्काः())
			append(bit);
	}

	std::सदिशम्<DriveChunk> ध्रुव &chunks() ध्रुव { pack(); प्रत्यागम chunks_; }
	std::सदिशम्<DriveBit> ध्रुव &द्वयङ्काः() ध्रुव { unpack(); प्रत्यागम द्वयङ्काः_2; }

	पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः_3; }

	शून्य append(DriveBit ध्रुव &bit);

	शून्य append(DriveChunk ध्रुव &chunk);

	शून्य pack() ध्रुव;

	शून्य unpack() ध्रुव;

	DriveBit &संक्रियकः[](पूर्णाङ्क सूचकांक)
	{
		log_assert(सूचकांक >= 0 && सूचकांक < आकार());
		unpack();
		प्रत्यागम द्वयङ्काः_2[सूचकांक];
	}

	ध्रुव DriveBit &संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव
	{
		log_assert(सूचकांक >= 0 && सूचकांक < आकार());
		unpack();
		प्रत्यागम द्वयङ्काः_2[सूचकांक];
	}

	शून्य clear()
	{
		chunks_.clear();
		द्वयङ्काः_2.clear();
		विस्तारः_3 = 0;
	}

	DriveSpec &संक्रियकः=(DriveChunk ध्रुव &chunk)
	{
		chunks_.clear();
		द्वयङ्काः_2.clear();
		append(chunk);
		प्रत्यागम *this;
	}

	DriveSpec &संक्रियकः=(DriveChunkWire ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &संक्रियकः=(Drive_Chunk_पत्तनम् ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &संक्रियकः=(DriveChunkMarker ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &संक्रियकः=(Drive_Chunk_गुणजः ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }

	DriveSpec &संक्रियकः=(DriveBit ध्रुव &bit)
	{
		chunks_.clear();
		द्वयङ्काः_2.clear();
		append(bit);
		प्रत्यागम *this;
	}

	DriveSpec &संक्रियकः=(DriveBitWire ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &संक्रियकः=(Drive_Bit_पत्तनम् ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &संक्रियकः=(DriveBitMarker ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &संक्रियकः=(Drive_Bit_गुणजः ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }

	शून्य updhash() ध्रुव {
		यदि (hash_ != 0)
			प्रत्यागम;
		pack();
		hash_ = सततम्_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool संक्रियकः==(DriveSpec ध्रुव &other) ध्रुव {
		updhash();
		other.updhash();
		यदि (आकार() != other.आकार() || hash_ != other.hash_)
			प्रत्यागम false;
		प्रत्यागम chunks() == other.chunks();
	}

गुप्त:
	शून्य compute_विस्तारः();
};



संरचना DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.सज्जता(); }
	DriverMap(Design *design) { celltypes.सज्जता(design); }

गुप्त:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	संरचना DriveBitId
	{
		पूर्णाङ्क id = -1;

		DriveBitId() {};

		DriveBitId(पूर्णाङ्क id) : id(id) { }

		bool संक्रियकः==(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id == other.id; }
		bool संक्रियकः!=(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id != other.id; }
		bool संक्रियकः<(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	संरचना DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_धाराः;
		dict<DriveBitId, DriveBitId> second_धाराः;
		dict<DriveBitId, pool<DriveBitId>> अधिकम्_धाराः;

		शून्य सङ्कलय_edge(DriveBitId src, DriveBitId dst);
		DriveBitId निष्कासय_edge(DriveBitId src);
		शून्य clear(DriveBitId src);
		bool अन्तर्भवति(DriveBitId src);
		पूर्णाङ्क गणनम्(DriveBitId src);

		DriveBitId at(DriveBitId src, पूर्णाङ्क सूचकांक);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, DriveBitId> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, DriveBitId> पत्तनम्_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<DriveBitId, DriveBit> drive_द्वयङ्काः;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<DriveBitId, DriveBit> isolated_drive_द्वयङ्काः;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	पूर्णाङ्क अग्र_विस्थापनम् = 1 + (पूर्णाङ्क)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<DriveBitId> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	DriveBitGraph connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	DriveBitGraph connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	DriveBitGraph connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<DriveBitId> oriented_present;


	गणना class Bit_बहुलकम् {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	Bit_बहुलकम् bit_बहुलकम्(DriveBit ध्रुव &bit);
	DriveBitId id_from_drive_bit(DriveBit ध्रुव &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	शून्य connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_directed_बफर(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_undirected(DriveBitId a_id, DriveBitId b_id);

सार्वजनिक:

	शून्य सङ्कलय(Module *module);

	// Add a single bit connection to the driver map.
	शून्य सङ्कलय(DriveBit ध्रुव &a, DriveBit ध्रुव &b);

	प्रतिरूपम्<typename T>
	स्थिर constexpr bool is_sig_type() {
		प्रत्यागम
			std::is_same<T, SigSpec>::मानम् ||
			std::is_same<T, SigChunk>::मानम् ||
			std::is_same<T, DriveSpec>::मानम् ||
			std::is_same<T, DriveChunk>::मानम् ||
			std::is_same<T, Drive_Chunk_पत्तनम्>::मानम् ||
			std::is_same<T, DriveChunkWire>::मानम् ||
			std::is_same<T, Const>::मानम्;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	प्रतिरूपम्<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	सङ्कलय(T ध्रुव &a, U ध्रुव &b)
	{
		log_assert(a.आकार() == b.आकार());
		प्रति (पूर्णाङ्क i = 0; i != Get_आकार(a); ++i)
			सङ्कलय(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	शून्य सङ्कलय(SigSpec ध्रुव &a, SigSpec ध्रुव &b);

गुप्त:
	शून्य सङ्कलय_पत्तनम्(Cell *cell, IdString पत्तनम्, SigSpec ध्रुव &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_आलोकितम्;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, पूर्णाङ्क> orient_undirected_distance;

	शून्य orient_undirected(DriveBitId id);

सार्वजनिक:
	DriveBit संक्रियकः()(DriveBit ध्रुव &bit);

	DriveSpec संक्रियकः()(DriveSpec spec);

गुप्त:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		प्रत्यागम wire->has_attribute(ID(keep));
	}
};

अन्तःस्थ Hasher DriveBitWire::hash_into(Hasher h) ध्रुव
{
	h.eat(wire->name);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher Drive_Bit_पत्तनम्::hash_into(Hasher h) ध्रुव
{
	h.eat(cell->name);
	h.eat(पत्तनम्);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveBitMarker::hash_into(Hasher h) ध्रुव
{
	h.eat(marker);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher Drive_Bit_गुणजः::hash_into(Hasher h) ध्रुव
{
	h.eat(गुणजः_3);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveBit::hash_into(Hasher h) ध्रुव
{
	विकल्प (type_) {
	पक्ष DriveType::NONE:
		h.eat(0);
		भङ्ग;
	पक्ष DriveType::CONSTANT:
		h.eat(constant_);
		भङ्ग;
	पक्ष DriveType::WIRE:
		h.eat(wire_);
		भङ्ग;
	पक्ष DriveType::पत्तनम्_2:
		h.eat(पत्तनम्_3);
		भङ्ग;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		भङ्ग;
	पक्ष DriveType::गुणजः:
		h.eat(गुणजः_3);
		भङ्ग;
	}
	h.eat(type_);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkWire::hash_into(Hasher h) ध्रुव
{
	h.eat(wire->name);
	h.eat(विस्तारः);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher Drive_Chunk_पत्तनम्::hash_into(Hasher h) ध्रुव
{
	h.eat(cell->name);
	h.eat(पत्तनम्);
	h.eat(विस्तारः);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkMarker::hash_into(Hasher h) ध्रुव
{
	h.eat(marker);
	h.eat(विस्तारः);
	h.eat(विस्थापनम्);
	प्रत्यागम h;
}

अन्तःस्थ Hasher Drive_Chunk_गुणजः::hash_into(Hasher h) ध्रुव
{
	h.eat(विस्तारः_3);
	h.eat(गुणजः_3);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunk::hash_into(Hasher h) ध्रुव
{
	विकल्प (type_) {
	पक्ष DriveType::NONE:
		h.eat(0);
		भङ्ग;
	पक्ष DriveType::CONSTANT:
		h.eat(constant_);
		भङ्ग;
	पक्ष DriveType::WIRE:
		h.eat(wire_);
		भङ्ग;
	पक्ष DriveType::पत्तनम्_2:
		h.eat(पत्तनम्_3);
		भङ्ग;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		भङ्ग;
	पक्ष DriveType::गुणजः:
		h.eat(गुणजः_3);
		भङ्ग;
	}
	h.eat(type_);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveSpec::hash_into(Hasher h) ध्रुव
{
	updhash();
	h.eat(hash_);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriverMap::DriveBitId::hash_into(Hasher h) ध्रुव
{
	h.eat(id);
	प्रत्यागम h;
}

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
