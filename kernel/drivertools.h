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

#ifndef DRIVERTOOLS_H
#define DRIVERTOOLS_H

#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ಸಂರಚನಾ DriveBit;

ಸಂರಚನಾ DriveChunkWire;
ಸಂರಚನಾ DriveChunkPort;
ಸಂರಚನಾ DriveChunkMarker;
ಸಂರಚನಾ DriveChunk;

ಸಂರಚನಾ DriveSpec;

std::string log_signal(DriveChunkWire ಧ್ರುವ &chunk);
std::string log_signal(DriveChunkPort ಧ್ರುವ &chunk);
std::string log_signal(DriveChunkMarker ಧ್ರುವ &chunk);
std::string log_signal(DriveChunk ಧ್ರುವ &chunk);
std::string log_signal(DriveSpec ಧ್ರುವ &chunk);

ಗಣನಾ class DriveType : unsigned ವರ್ಣ
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

ಸಂರಚನಾ DriveBitWire
{
	Wire *wire;
	ಪೂರ್ಣಾಙ್ಕ offset;

	DriveBitWire(Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset) : wire(wire), offset(offset) {}

	bool operator==(ಧ್ರುವ DriveBitWire &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ wire == other.wire && offset == other.offset;
	}

	bool operator<(ಧ್ರುವ DriveBitWire &other) ಧ್ರುವ
	{
		ಯದಿ (wire != other.wire)
			ಪ್ರತ್ಯಾಗಮ wire->name < other.wire->name;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;


	operator SigBit() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ SigBit(wire, offset);
	}
};

ಸಂರಚನಾ DriveBitPort
{
	Cell *cell;
	IdString port;
	ಪೂರ್ಣಾಙ್ಕ offset;

	DriveBitPort(Cell *cell, IdString port, ಪೂರ್ಣಾಙ್ಕ offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ಧ್ರುವ DriveBitPort &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ಧ್ರುವ DriveBitPort &other) ಧ್ರುವ
	{
		ಯದಿ (cell != other.cell)
			ಪ್ರತ್ಯಾಗಮ cell->name < other.cell->name;
		ಯದಿ (port != other.port)
			ಪ್ರತ್ಯಾಗಮ port < other.port;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

};


ಸಂರಚನಾ DriveBitMarker
{
	ಪೂರ್ಣಾಙ್ಕ marker;
	ಪೂರ್ಣಾಙ್ಕ offset;

	DriveBitMarker(ಪೂರ್ಣಾಙ್ಕ marker, ಪೂರ್ಣಾಙ್ಕ offset) : marker(marker), offset(offset) {}

	bool operator==(ಧ್ರುವ DriveBitMarker &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ marker == other.marker && offset == other.offset;
	}

	bool operator<(ಧ್ರುವ DriveBitMarker &other) ಧ್ರುವ
	{
		ಯದಿ (marker != other.marker)
			ಪ್ರತ್ಯಾಗಮ marker < other.marker;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

};

ಸಂರಚನಾ DriveBitMultiple
{
ಗುಪ್ತ:
	pool<DriveBit> multiple_;

ಸಾರ್ವಜನಿಕ:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ಧ್ರುವ &single);

	pool<DriveBit> ಧ್ರುವ &multiple() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ multiple_; }

	ಶೂನ್ಯ merge(DriveBitMultiple ಧ್ರುವ &other)
	{
		ಪ್ರತಿ (DriveBit ಧ್ರುವ &single : other.multiple_)
			merge(single);
	}

	ಶೂನ್ಯ merge(DriveBitMultiple &&other)
	{
		ಪ್ರತಿ (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	ಶೂನ್ಯ merge(DriveBit ಧ್ರುವ &single);
	ಶೂನ್ಯ merge(DriveBit &&single);

	bool operator==(ಧ್ರುವ DriveBitMultiple &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
};

ಸಂರಚನಾ DriveBit
{
ಗುಪ್ತ:
	DriveType type_ = DriveType::NONE;
	ಸಙ್ಘ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
ಸಾರ್ವಜನಿಕ:
	DriveBit()  {}

	DriveBit(SigBit ಧ್ರುವ &bit);

	DriveBit(DriveBit ಧ್ರುವ &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ಧ್ರುವ &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ಧ್ರುವ &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ಧ್ರುವ &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ಧ್ರುವ &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	ಶೂನ್ಯ set_none()
	{
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				wire_.~DriveBitWire();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				port_.~DriveBitPort();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				marker_.~DriveBitMarker();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				ಭಙ್ಗ;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ಧ್ರುವ &other)
	{
		ವಿಕಲ್ಪ (other.type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				set_none();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				*this = other.constant_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				*this = other.wire_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				*this = other.port_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				*this = other.marker_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				*this = other.multiple_;
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		ವಿಕಲ್ಪ (other.type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				set_none();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				*this = std::move(other.wire_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				*this = std::move(other.port_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				*this = std::move(other.marker_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitWire ಧ್ರುವ &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitPort ಧ್ರುವ &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitMarker ಧ್ರುವ &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitMultiple ಧ್ರುವ &multiple)
	{
		set_none();
		ಯದಿ (multiple.multiple().empty())
			ಪ್ರತ್ಯಾಗಮ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		ಯದಿ (multiple.multiple().empty())
			ಪ್ರತ್ಯಾಗಮ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

	bool operator==(ಧ್ರುವ DriveBit &other) ಧ್ರುವ
	{
		ಯದಿ (type_ != other.type_)
			ಪ್ರತ್ಯಾಗಮ false;

		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ true;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_ == other.constant_;
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_ == other.wire_;
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_ == other.port_;
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_ == other.marker_;
			ಪಕ್ಷ DriveType::MULTIPLE:
				ಪ್ರತ್ಯಾಗಮ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ಧ್ರುವ DriveBit &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ !(*this == other);
	}

	bool operator<(ಧ್ರುವ DriveBit &other) ಧ್ರುವ
	{
		ಯದಿ (type_ != other.type_)
			ಪ್ರತ್ಯಾಗಮ type_ < other.type_;
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ false;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_ < other.constant_;
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_ < other.wire_;
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_ < other.port_;
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_ < other.marker_;
			ಪಕ್ಷ DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_; }

	bool is_none() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::NONE; }
	bool is_constant() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::CONSTANT; }
	bool is_wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::WIRE; }
	bool is_port() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::PORT; }
	bool is_marker() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::MARKER; }
	bool is_multiple() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); ಪ್ರತ್ಯಾಗಮ constant_; }
	State ಧ್ರುವ &constant() ಧ್ರುವ { log_assert(is_constant()); ಪ್ರತ್ಯಾಗಮ constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); ಪ್ರತ್ಯಾಗಮ wire_; }
	DriveBitWire ಧ್ರುವ &wire() ಧ್ರುವ { log_assert(is_wire()); ಪ್ರತ್ಯಾಗಮ wire_; }
	DriveBitPort &port() { log_assert(is_port()); ಪ್ರತ್ಯಾಗಮ port_; }
	DriveBitPort ಧ್ರುವ &port() ಧ್ರುವ { log_assert(is_port()); ಪ್ರತ್ಯಾಗಮ port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); ಪ್ರತ್ಯಾಗಮ marker_; }
	DriveBitMarker ಧ್ರುವ &marker() ಧ್ರುವ { log_assert(is_marker()); ಪ್ರತ್ಯಾಗಮ marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); ಪ್ರತ್ಯಾಗಮ multiple_; }
	DriveBitMultiple ಧ್ರುವ &multiple() ಧ್ರುವ { log_assert(is_multiple()); ಪ್ರತ್ಯಾಗಮ multiple_; }

	ಶೂನ್ಯ merge(DriveBit ಧ್ರುವ &other);

};

ಅನ್ತಃಸ್ಥ DriveBitMultiple::DriveBitMultiple() {}
ಅನ್ತಃಸ್ಥ DriveBitMultiple::DriveBitMultiple(DriveBit ಧ್ರುವ &single)
{
	multiple_.emplace(single);
}

ಸಂರಚನಾ DriveChunkWire
{
	Wire *wire;
	ಪೂರ್ಣಾಙ್ಕ offset;
	ಪೂರ್ಣಾಙ್ಕ width;

	DriveChunkWire(Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ಧ್ರುವ &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width; }

	DriveBitWire operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
	{
		log_assert(i >= 0 && i < width);
		ಪ್ರತ್ಯಾಗಮ DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ಧ್ರುವ &bit) ಧ್ರುವ;
	bool try_append(DriveBitWire ಧ್ರುವ &bit);
	bool try_append(DriveChunkWire ಧ್ರುವ &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ offset == 0 && width == wire->width; }

	bool operator==(ಧ್ರುವ DriveChunkWire &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ಧ್ರುವ DriveChunkWire &other) ಧ್ರುವ
	{
		ಯದಿ (wire != other.wire)
			ಪ್ರತ್ಯಾಗಮ wire->name < other.wire->name;
		ಯದಿ (width != other.width)
			ಪ್ರತ್ಯಾಗಮ width < other.width;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

	explicit operator SigChunk() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ SigChunk(wire, offset, width);
	}
};

ಸಂರಚನಾ DriveChunkPort
{
	Cell *cell;
	IdString port;
	ಪೂರ್ಣಾಙ್ಕ offset;
	ಪೂರ್ಣಾಙ್ಕ width;

	DriveChunkPort(Cell *cell, IdString port, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ಧ್ರುವ &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ಧ್ರುವ &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width; }

	DriveBitPort operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
	{
		log_assert(i >= 0 && i < width);
		ಪ್ರತ್ಯಾಗಮ DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ಧ್ರುವ &bit) ಧ್ರುವ;
	bool try_append(DriveBitPort ಧ್ರುವ &bit);
	bool try_append(DriveChunkPort ಧ್ರುವ &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ಧ್ರುವ DriveChunkPort &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ಧ್ರುವ DriveChunkPort &other) ಧ್ರುವ
	{
		ಯದಿ (cell != other.cell)
			ಪ್ರತ್ಯಾಗಮ cell->name < other.cell->name;
		ಯದಿ (port != other.port)
			ಪ್ರತ್ಯಾಗಮ port < other.port;
		ಯದಿ (width != other.width)
			ಪ್ರತ್ಯಾಗಮ width < other.width;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
};


ಸಂರಚನಾ DriveChunkMarker
{
	ಪೂರ್ಣಾಙ್ಕ marker;
	ಪೂರ್ಣಾಙ್ಕ offset;
	ಪೂರ್ಣಾಙ್ಕ width;

	DriveChunkMarker(ಪೂರ್ಣಾಙ್ಕ marker, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ಧ್ರುವ &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width; }

	DriveBitMarker operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
	{
		log_assert(i >= 0 && i < width);
		ಪ್ರತ್ಯಾಗಮ DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ಧ್ರುವ &bit) ಧ್ರುವ;
	bool try_append(DriveBitMarker ಧ್ರುವ &bit);
	bool try_append(DriveChunkMarker ಧ್ರುವ &chunk);

	bool operator==(ಧ್ರುವ DriveChunkMarker &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ಧ್ರುವ DriveChunkMarker &other) ಧ್ರುವ
	{
		ಯದಿ (marker != other.marker)
			ಪ್ರತ್ಯಾಗಮ marker < other.marker;
		ಯದಿ (width != other.width)
			ಪ್ರತ್ಯಾಗಮ width < other.width;
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
};

ಸಂರಚನಾ DriveChunkMultiple
{
ಗುಪ್ತ:
	mutable pool<DriveChunk> multiple_;
	ಪೂರ್ಣಾಙ್ಕ width_;

ಸಾರ್ವಜನಿಕ:
	pool<DriveChunk> ಧ್ರುವ &multiple() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ multiple_; }

	DriveChunkMultiple(DriveBitMultiple ಧ್ರುವ &bit);

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width_; }

	DriveBitMultiple operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ;

	bool can_append(DriveBitMultiple ಧ್ರುವ &bit) ಧ್ರುವ;

	bool try_append(DriveBitMultiple ಧ್ರುವ &bit);


	bool can_append(DriveChunkMultiple ಧ್ರುವ &bit) ಧ್ರುವ;

	bool try_append(DriveChunkMultiple ಧ್ರುವ &bit);

	bool operator==(ಧ್ರುವ DriveChunkMultiple &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ಧ್ರುವ DriveChunkMultiple &other) ಧ್ರುವ
	{
		ಯದಿ (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		ಪ್ರತ್ಯಾಗಮ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
};

ಸಂರಚನಾ DriveChunk
{
ಗುಪ್ತ:
	DriveType type_ = DriveType::NONE;
	ಸಙ್ಘ
	{
		ಪೂರ್ಣಾಙ್ಕ none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

ಸಾರ್ವಜನಿಕ:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ಧ್ರುವ &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ಧ್ರುವ &other) { *this = other; }

	DriveChunk(Const ಧ್ರುವ &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ಧ್ರುವ &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ಧ್ರುವ &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ಧ್ರುವ &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ಧ್ರುವ &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
	{
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ DriveBit();
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_[i];
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_[i];
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_[i];
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_[i];
			ಪಕ್ಷ DriveType::MULTIPLE:
				ಪ್ರತ್ಯಾಗಮ multiple_[i];
		}
		log_abort();
	}

	ಶೂನ್ಯ set_none(ಪೂರ್ಣಾಙ್ಕ width = 0)
	{
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				none_ = width;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				constant_.~Const();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				wire_.~DriveChunkWire();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				port_.~DriveChunkPort();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				ಭಙ್ಗ;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ಧ್ರುವ &other)
	{
		ವಿಕಲ್ಪ (other.type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				set_none(other.none_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				*this = other.constant_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				*this = other.wire_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				*this = other.port_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				*this = other.marker_;
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				*this = other.multiple_;
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		ವಿಕಲ್ಪ (other.type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				set_none(other.none_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				*this = std::move(other.wire_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				*this = std::move(other.port_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				*this = std::move(other.marker_);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(Const ಧ್ರುವ &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkWire ಧ್ರುವ &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkPort ಧ್ರುವ &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkMarker ಧ್ರುವ &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ಧ್ರುವ &multiple)
	{
		set_none(multiple.size());
		ಯದಿ (multiple.multiple().empty())
			ಪ್ರತ್ಯಾಗಮ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		ಯದಿ (multiple.multiple().empty())
			ಪ್ರತ್ಯಾಗಮ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveChunk &operator=(DriveBit ಧ್ರುವ &other)
	{
		ವಿಕಲ್ಪ (other.type())
		{
			ಪಕ್ಷ DriveType::NONE:
				set_none(1);
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::CONSTANT:
				*this = Const(other.constant());
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT:
				*this = DriveChunkPort(other.port());
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				ಭಙ್ಗ;
		}
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	bool can_append(DriveBit ಧ್ರುವ &bit) ಧ್ರುವ;
	bool try_append(DriveBit ಧ್ರುವ &bit);
	bool try_append(DriveChunk ಧ್ರುವ &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

	bool operator==(ಧ್ರುವ DriveChunk &other) ಧ್ರುವ
	{
		ಯದಿ (type_ != other.type_)
			ಪ್ರತ್ಯಾಗಮ false;

		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ true;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_ == other.constant_;
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_ == other.wire_;
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_ == other.port_;
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_ == other.marker_;
			ಪಕ್ಷ DriveType::MULTIPLE:
				ಪ್ರತ್ಯಾಗಮ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ಧ್ರುವ DriveChunk &other) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ !(*this == other);
	}

	bool operator<(ಧ್ರುವ DriveChunk &other) ಧ್ರುವ
	{
		ಯದಿ (type_ != other.type_)
			ಪ್ರತ್ಯಾಗಮ type_ < other.type_;
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ false;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_ < other.constant_;
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_ < other.wire_;
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_ < other.port_;
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_ < other.marker_;
			ಪಕ್ಷ DriveType::MULTIPLE:
				ಪ್ರತ್ಯಾಗಮ multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_; }

	bool is_none() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::NONE; }
	bool is_constant() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::CONSTANT; }
	bool is_wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::WIRE; }
	bool is_port() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::PORT; }
	bool is_marker() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::MARKER; }
	bool is_multiple() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); ಪ್ರತ್ಯಾಗಮ constant_; }
	Const ಧ್ರುವ &constant() ಧ್ರುವ { log_assert(is_constant()); ಪ್ರತ್ಯಾಗಮ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ಪ್ರತ್ಯಾಗಮ wire_; }
	DriveChunkWire ಧ್ರುವ &wire() ಧ್ರುವ { log_assert(is_wire()); ಪ್ರತ್ಯಾಗಮ wire_; }
	DriveChunkPort &port() { log_assert(is_port()); ಪ್ರತ್ಯಾಗಮ port_; }
	DriveChunkPort ಧ್ರುವ &port() ಧ್ರುವ { log_assert(is_port()); ಪ್ರತ್ಯಾಗಮ port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ಪ್ರತ್ಯಾಗಮ marker_; }
	DriveChunkMarker ಧ್ರುವ &marker() ಧ್ರುವ { log_assert(is_marker()); ಪ್ರತ್ಯಾಗಮ marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); ಪ್ರತ್ಯಾಗಮ multiple_; }
	DriveChunkMultiple ಧ್ರುವ &multiple() ಧ್ರುವ { log_assert(is_multiple()); ಪ್ರತ್ಯಾಗಮ multiple_; }


	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ
	{
		ವಿಕಲ್ಪ (type_)
		{
			ಪಕ್ಷ DriveType::NONE:
				ಪ್ರತ್ಯಾಗಮ none_;
			ಪಕ್ಷ DriveType::CONSTANT:
				ಪ್ರತ್ಯಾಗಮ constant_.size();
			ಪಕ್ಷ DriveType::WIRE:
				ಪ್ರತ್ಯಾಗಮ wire_.size();
			ಪಕ್ಷ DriveType::PORT:
				ಪ್ರತ್ಯಾಗಮ port_.size();
			ಪಕ್ಷ DriveType::MARKER:
				ಪ್ರತ್ಯಾಗಮ marker_.size();
			ಪಕ್ಷ DriveType::MULTIPLE:
				ಪ್ರತ್ಯಾಗಮ multiple_.size();
		}
		log_abort();
	}
};

ಅನ್ತಃಸ್ಥ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ಧ್ರುವ &bit)
	: width_(1)
{
	ಪ್ರತಿ (auto ಧ್ರುವ &bit : bit.multiple())
		multiple_.emplace(bit);
}

ಸಂರಚನಾ DriveSpec
{
ಗುಪ್ತ:
	ಪೂರ್ಣಾಙ್ಕ width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned ಪೂರ್ಣಾಙ್ಕ hash_ = 0;
ಸಾರ್ವಜನಿಕ:

	ಅನ್ತಃಸ್ಥ bool packed() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ಧ್ರುವ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ಧ್ರುವ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ಧ್ರುವ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ಧ್ರುವ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ಧ್ರುವ &chunk) { *this = chunk; }

	DriveSpec(DriveBit ಧ್ರುವ &bit) { *this = bit; }
	DriveSpec(DriveBitWire ಧ್ರುವ &bit) { *this = bit; }
	DriveSpec(DriveBitPort ಧ್ರುವ &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ಧ್ರುವ &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ಧ್ರುವ &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ಧ್ರುವ &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ಧ್ರುವ &bits)
	{
		ಪ್ರತಿ (auto ಧ್ರುವ &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ಧ್ರುವ &sig)
	{
		// TODO: converting one chunk at a time would be faster
		ಪ್ರತಿ (auto ಧ್ರುವ &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ಧ್ರುವ &chunks() ಧ್ರುವ { pack(); ಪ್ರತ್ಯಾಗಮ chunks_; }
	std::vector<DriveBit> ಧ್ರುವ &bits() ಧ್ರುವ { unpack(); ಪ್ರತ್ಯಾಗಮ bits_; }

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width_; }

	ಶೂನ್ಯ append(DriveBit ಧ್ರುವ &bit);

	ಶೂನ್ಯ append(DriveChunk ಧ್ರುವ &chunk);

	ಶೂನ್ಯ pack() ಧ್ರುವ;

	ಶೂನ್ಯ unpack() ಧ್ರುವ;

	DriveBit &operator[](ಪೂರ್ಣಾಙ್ಕ index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ಪ್ರತ್ಯಾಗಮ bits_[index];
	}

	ಧ್ರುವ DriveBit &operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ
	{
		log_assert(index >= 0 && index < size());
		unpack();
		ಪ್ರತ್ಯಾಗಮ bits_[index];
	}

	ಶೂನ್ಯ clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ಧ್ರುವ &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveSpec &operator=(DriveChunkWire ಧ್ರುವ &chunk) { ಪ್ರತ್ಯಾಗಮ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ಧ್ರುವ &chunk) { ಪ್ರತ್ಯಾಗಮ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ಧ್ರುವ &chunk) { ಪ್ರತ್ಯಾಗಮ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ಧ್ರುವ &chunk) { ಪ್ರತ್ಯಾಗಮ *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ಧ್ರುವ &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	DriveSpec &operator=(DriveBitWire ಧ್ರುವ &bit) { ಪ್ರತ್ಯಾಗಮ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ಧ್ರುವ &bit) { ಪ್ರತ್ಯಾಗಮ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ಧ್ರುವ &bit) { ಪ್ರತ್ಯಾಗಮ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ಧ್ರುವ &bit) { ಪ್ರತ್ಯಾಗಮ *this = DriveBit(bit); }

	ಶೂನ್ಯ updhash() ಧ್ರುವ {
		ಯದಿ (hash_ != 0)
			ಪ್ರತ್ಯಾಗಮ;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;

	bool operator==(DriveSpec ಧ್ರುವ &other) ಧ್ರುವ {
		updhash();
		other.updhash();
		ಯದಿ (size() != other.size() || hash_ != other.hash_)
			ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ chunks() == other.chunks();
	}

ಗುಪ್ತ:
	ಶೂನ್ಯ compute_width();
};



ಸಂರಚನಾ DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

ಗುಪ್ತ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ಸಂರಚನಾ DriveBitId
	{
		ಪೂರ್ಣಾಙ್ಕ id = -1;

		DriveBitId() {};

		DriveBitId(ಪೂರ್ಣಾಙ್ಕ id) : id(id) { }

		bool operator==(ಧ್ರುವ DriveBitId &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ id == other.id; }
		bool operator!=(ಧ್ರುವ DriveBitId &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ id != other.id; }
		bool operator<(ಧ್ರುವ DriveBitId &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ಸಂರಚನಾ DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		ಶೂನ್ಯ add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId ತೆಗೆ_edge(DriveBitId src);
		ಶೂನ್ಯ clear(DriveBitId src);
		bool contains(DriveBitId src);
		ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(DriveBitId src);

		DriveBitId at(DriveBitId src, ಪೂರ್ಣಾಙ್ಕ index);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, DriveBitId> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, DriveBitId> port_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<DriveBitId, DriveBit> drive_bits;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<DriveBitId, DriveBit> isolated_drive_bits;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	ಪೂರ್ಣಾಙ್ಕ next_offset = 1 + (ಪೂರ್ಣಾಙ್ಕ)State::Sm;

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


	ಗಣನಾ class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit ಧ್ರುವ &bit);
	DriveBitId id_from_drive_bit(DriveBit ಧ್ರುವ &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	ಶೂನ್ಯ connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	ಶೂನ್ಯ connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	ಶೂನ್ಯ connect_undirected(DriveBitId a_id, DriveBitId b_id);

ಸಾರ್ವಜನಿಕ:

	ಶೂನ್ಯ add(Module *module);

	// Add a single bit connection to the driver map.
	ಶೂನ್ಯ add(DriveBit ಧ್ರುವ &a, DriveBit ಧ್ರುವ &b);

	template<typename T>
	ಸ್ಥಿರ constexpr bool is_sig_type() {
		ಪ್ರತ್ಯಾಗಮ
			std::is_same<T, SigSpec>::ಮೌಲ್ಯ ||
			std::is_same<T, SigChunk>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveSpec>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveChunk>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveChunkPort>::ಮೌಲ್ಯ ||
			std::is_same<T, DriveChunkWire>::ಮೌಲ್ಯ ||
			std::is_same<T, Const>::ಮೌಲ್ಯ;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ಧ್ರುವ &a, U ಧ್ರುವ &b)
	{
		log_assert(a.size() == b.size());
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	ಶೂನ್ಯ add(SigSpec ಧ್ರುವ &a, SigSpec ಧ್ರುವ &b);

ಗುಪ್ತ:
	ಶೂನ್ಯ add_port(Cell *cell, IdString port, SigSpec ಧ್ರುವ &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, ಪೂರ್ಣಾಙ್ಕ> orient_undirected_distance;

	ಶೂನ್ಯ orient_undirected(DriveBitId id);

ಸಾರ್ವಜನಿಕ:
	DriveBit operator()(DriveBit ಧ್ರುವ &bit);

	DriveSpec operator()(DriveSpec spec);

ಗುಪ್ತ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ಪ್ರತ್ಯಾಗಮ wire->has_attribute(ID(keep));
	}
};

ಅನ್ತಃಸ್ಥ Hasher DriveBitWire::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(wire->name);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveBitPort::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveBitMarker::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(marker);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveBitMultiple::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(multiple_);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveBit::hash_into(Hasher h) ಧ್ರುವ
{
	ವಿಕಲ್ಪ (type_) {
	ಪಕ್ಷ DriveType::NONE:
		h.eat(0);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::CONSTANT:
		h.eat(constant_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::WIRE:
		h.eat(wire_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::PORT:
		h.eat(port_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::MARKER:
		h.eat(marker_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::MULTIPLE:
		h.eat(multiple_);
		ಭಙ್ಗ;
	}
	h.eat(type_);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveChunkWire::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveChunkPort::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveChunkMarker::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveChunkMultiple::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(width_);
	h.eat(multiple_);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveChunk::hash_into(Hasher h) ಧ್ರುವ
{
	ವಿಕಲ್ಪ (type_) {
	ಪಕ್ಷ DriveType::NONE:
		h.eat(0);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::CONSTANT:
		h.eat(constant_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::WIRE:
		h.eat(wire_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::PORT:
		h.eat(port_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::MARKER:
		h.eat(marker_);
		ಭಙ್ಗ;
	ಪಕ್ಷ DriveType::MULTIPLE:
		h.eat(multiple_);
		ಭಙ್ಗ;
	}
	h.eat(type_);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriveSpec::hash_into(Hasher h) ಧ್ರುವ
{
	updhash();
	h.eat(hash_);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ Hasher DriverMap::DriveBitId::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(id);
	ಪ್ರತ್ಯಾಗಮ h;
}

YOSYS_NAMESPACE_END

#endif
