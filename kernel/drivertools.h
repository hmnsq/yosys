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

#અન્તર્ભાવ <string>
#અન્તર્ભાવ <type_traits>

#અન્તર્ભાવ "kernel/rtlil.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

સંરચના DriveBit;

સંરચના DriveChunkWire;
સંરચના DriveChunkPort;
સંરચના DriveChunkMarker;
સંરચના DriveChunk;

સંરચના DriveSpec;

std::string log_signal(DriveChunkWire અચલ &chunk);
std::string log_signal(DriveChunkPort અચલ &chunk);
std::string log_signal(DriveChunkMarker અચલ &chunk);
std::string log_signal(DriveChunk અચલ &chunk);
std::string log_signal(DriveSpec અચલ &chunk);

ગણના class DriveType : unsigned અક્ષર
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

સંરચના DriveBitWire
{
	Wire *wire;
	પૂર્ણાંક offset;

	DriveBitWire(Wire *wire, પૂર્ણાંક offset) : wire(wire), offset(offset) {}

	bool operator==(અચલ DriveBitWire &other) અચલ
	{
		પાછા wire == other.wire && offset == other.offset;
	}

	bool operator<(અચલ DriveBitWire &other) અચલ
	{
		જો (wire != other.wire)
			પાછા wire->name < other.wire->name;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;


	operator SigBit() અચલ
	{
		પાછા SigBit(wire, offset);
	}
};

સંરચના DriveBitPort
{
	Cell *cell;
	IdString port;
	પૂર્ણાંક offset;

	DriveBitPort(Cell *cell, IdString port, પૂર્ણાંક offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(અચલ DriveBitPort &other) અચલ
	{
		પાછા cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(અચલ DriveBitPort &other) અચલ
	{
		જો (cell != other.cell)
			પાછા cell->name < other.cell->name;
		જો (port != other.port)
			પાછા port < other.port;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

};


સંરચના DriveBitMarker
{
	પૂર્ણાંક marker;
	પૂર્ણાંક offset;

	DriveBitMarker(પૂર્ણાંક marker, પૂર્ણાંક offset) : marker(marker), offset(offset) {}

	bool operator==(અચલ DriveBitMarker &other) અચલ
	{
		પાછા marker == other.marker && offset == other.offset;
	}

	bool operator<(અચલ DriveBitMarker &other) અચલ
	{
		જો (marker != other.marker)
			પાછા marker < other.marker;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

};

સંરચના DriveBitMultiple
{
ખાનગી:
	pool<DriveBit> multiple_;

જાહેર:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit અચલ &single);

	pool<DriveBit> અચલ &multiple() અચલ { પાછા multiple_; }

	શૂન્ય merge(DriveBitMultiple અચલ &other)
	{
		માટે (DriveBit અચલ &single : other.multiple_)
			merge(single);
	}

	શૂન્ય merge(DriveBitMultiple &&other)
	{
		માટે (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	શૂન્ય merge(DriveBit અચલ &single);
	શૂન્ય merge(DriveBit &&single);

	bool operator==(અચલ DriveBitMultiple &other) અચલ
	{
		પાછા multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
};

સંરચના DriveBit
{
ખાનગી:
	DriveType type_ = DriveType::NONE;
	સંઘ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
જાહેર:
	DriveBit()  {}

	DriveBit(SigBit અચલ &bit);

	DriveBit(DriveBit અચલ &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire અચલ &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort અચલ &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker અચલ &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple અચલ &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	શૂન્ય set_none()
	{
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				તોડો;
			સ્થિતિ DriveType::WIRE:
				wire_.~DriveBitWire();
				તોડો;
			સ્થિતિ DriveType::PORT:
				port_.~DriveBitPort();
				તોડો;
			સ્થિતિ DriveType::MARKER:
				marker_.~DriveBitMarker();
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				તોડો;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit અચલ &other)
	{
		પસંદગી (other.type_)
		{
			સ્થિતિ DriveType::NONE:
				set_none();
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				*this = other.constant_;
				તોડો;
			સ્થિતિ DriveType::WIRE:
				*this = other.wire_;
				તોડો;
			સ્થિતિ DriveType::PORT:
				*this = other.port_;
				તોડો;
			સ્થિતિ DriveType::MARKER:
				*this = other.marker_;
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				*this = other.multiple_;
				તોડો;
		}
		પાછા *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		પસંદગી (other.type_)
		{
			સ્થિતિ DriveType::NONE:
				set_none();
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				તોડો;
			સ્થિતિ DriveType::WIRE:
				*this = std::move(other.wire_);
				તોડો;
			સ્થિતિ DriveType::PORT:
				*this = std::move(other.port_);
				તોડો;
			સ્થિતિ DriveType::MARKER:
				*this = std::move(other.marker_);
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				તોડો;
		}
		પાછા *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitWire અચલ &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitPort અચલ &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitMarker અચલ &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitMultiple અચલ &multiple)
	{
		set_none();
		જો (multiple.multiple().empty())
			પાછા *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		પાછા *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		જો (multiple.multiple().empty())
			પાછા *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		પાછા *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

	bool operator==(અચલ DriveBit &other) અચલ
	{
		જો (type_ != other.type_)
			પાછા false;

		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા true;
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_ == other.constant_;
			સ્થિતિ DriveType::WIRE:
				પાછા wire_ == other.wire_;
			સ્થિતિ DriveType::PORT:
				પાછા port_ == other.port_;
			સ્થિતિ DriveType::MARKER:
				પાછા marker_ == other.marker_;
			સ્થિતિ DriveType::MULTIPLE:
				પાછા multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(અચલ DriveBit &other) અચલ
	{
		પાછા !(*this == other);
	}

	bool operator<(અચલ DriveBit &other) અચલ
	{
		જો (type_ != other.type_)
			પાછા type_ < other.type_;
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા false;
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_ < other.constant_;
			સ્થિતિ DriveType::WIRE:
				પાછા wire_ < other.wire_;
			સ્થિતિ DriveType::PORT:
				પાછા port_ < other.port_;
			સ્થિતિ DriveType::MARKER:
				પાછા marker_ < other.marker_;
			સ્થિતિ DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() અચલ { પાછા type_; }

	bool is_none() અચલ { પાછા type_ == DriveType::NONE; }
	bool is_constant() અચલ { પાછા type_ == DriveType::CONSTANT; }
	bool is_wire() અચલ { પાછા type_ == DriveType::WIRE; }
	bool is_port() અચલ { પાછા type_ == DriveType::PORT; }
	bool is_marker() અચલ { પાછા type_ == DriveType::MARKER; }
	bool is_multiple() અચલ { પાછા type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); પાછા constant_; }
	State અચલ &constant() અચલ { log_assert(is_constant()); પાછા constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); પાછા wire_; }
	DriveBitWire અચલ &wire() અચલ { log_assert(is_wire()); પાછા wire_; }
	DriveBitPort &port() { log_assert(is_port()); પાછા port_; }
	DriveBitPort અચલ &port() અચલ { log_assert(is_port()); પાછા port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); પાછા marker_; }
	DriveBitMarker અચલ &marker() અચલ { log_assert(is_marker()); પાછા marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); પાછા multiple_; }
	DriveBitMultiple અચલ &multiple() અચલ { log_assert(is_multiple()); પાછા multiple_; }

	શૂન્ય merge(DriveBit અચલ &other);

};

અંતઃરેખ DriveBitMultiple::DriveBitMultiple() {}
અંતઃરેખ DriveBitMultiple::DriveBitMultiple(DriveBit અચલ &single)
{
	multiple_.emplace(single);
}

સંરચના DriveChunkWire
{
	Wire *wire;
	પૂર્ણાંક offset;
	પૂર્ણાંક width;

	DriveChunkWire(Wire *wire, પૂર્ણાંક offset, પૂર્ણાંક width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire અચલ &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	પૂર્ણાંક size() અચલ { પાછા width; }

	DriveBitWire operator[](પૂર્ણાંક i) અચલ
	{
		log_assert(i >= 0 && i < width);
		પાછા DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire અચલ &bit) અચલ;
	bool try_append(DriveBitWire અચલ &bit);
	bool try_append(DriveChunkWire અચલ &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() અચલ { પાછા offset == 0 && width == wire->width; }

	bool operator==(અચલ DriveChunkWire &other) અચલ
	{
		પાછા wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(અચલ DriveChunkWire &other) અચલ
	{
		જો (wire != other.wire)
			પાછા wire->name < other.wire->name;
		જો (width != other.width)
			પાછા width < other.width;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

	explicit operator SigChunk() અચલ
	{
		પાછા SigChunk(wire, offset, width);
	}
};

સંરચના DriveChunkPort
{
	Cell *cell;
	IdString port;
	પૂર્ણાંક offset;
	પૂર્ણાંક width;

	DriveChunkPort(Cell *cell, IdString port, પૂર્ણાંક offset, પૂર્ણાંક width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> અચલ &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort અચલ &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	પૂર્ણાંક size() અચલ { પાછા width; }

	DriveBitPort operator[](પૂર્ણાંક i) અચલ
	{
		log_assert(i >= 0 && i < width);
		પાછા DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort અચલ &bit) અચલ;
	bool try_append(DriveBitPort અચલ &bit);
	bool try_append(DriveChunkPort અચલ &chunk);

	// Whether this chunk is a whole port
	bool is_whole() અચલ
	{
		પાછા offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(અચલ DriveChunkPort &other) અચલ
	{
		પાછા cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(અચલ DriveChunkPort &other) અચલ
	{
		જો (cell != other.cell)
			પાછા cell->name < other.cell->name;
		જો (port != other.port)
			પાછા port < other.port;
		જો (width != other.width)
			પાછા width < other.width;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
};


સંરચના DriveChunkMarker
{
	પૂર્ણાંક marker;
	પૂર્ણાંક offset;
	પૂર્ણાંક width;

	DriveChunkMarker(પૂર્ણાંક marker, પૂર્ણાંક offset, પૂર્ણાંક width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker અચલ &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	પૂર્ણાંક size() અચલ { પાછા width; }

	DriveBitMarker operator[](પૂર્ણાંક i) અચલ
	{
		log_assert(i >= 0 && i < width);
		પાછા DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker અચલ &bit) અચલ;
	bool try_append(DriveBitMarker અચલ &bit);
	bool try_append(DriveChunkMarker અચલ &chunk);

	bool operator==(અચલ DriveChunkMarker &other) અચલ
	{
		પાછા marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(અચલ DriveChunkMarker &other) અચલ
	{
		જો (marker != other.marker)
			પાછા marker < other.marker;
		જો (width != other.width)
			પાછા width < other.width;
		પાછા offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
};

સંરચના DriveChunkMultiple
{
ખાનગી:
	mutable pool<DriveChunk> multiple_;
	પૂર્ણાંક width_;

જાહેર:
	pool<DriveChunk> અચલ &multiple() અચલ { પાછા multiple_; }

	DriveChunkMultiple(DriveBitMultiple અચલ &bit);

	પૂર્ણાંક size() અચલ { પાછા width_; }

	DriveBitMultiple operator[](પૂર્ણાંક i) અચલ;

	bool can_append(DriveBitMultiple અચલ &bit) અચલ;

	bool try_append(DriveBitMultiple અચલ &bit);


	bool can_append(DriveChunkMultiple અચલ &bit) અચલ;

	bool try_append(DriveChunkMultiple અચલ &bit);

	bool operator==(અચલ DriveChunkMultiple &other) અચલ
	{
		પાછા width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(અચલ DriveChunkMultiple &other) અચલ
	{
		જો (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		પાછા false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
};

સંરચના DriveChunk
{
ખાનગી:
	DriveType type_ = DriveType::NONE;
	સંઘ
	{
		પૂર્ણાંક none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

જાહેર:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk અચલ &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit અચલ &other) { *this = other; }

	DriveChunk(Const અચલ &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire અચલ &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort અચલ &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker અચલ &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple અચલ &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](પૂર્ણાંક i) અચલ
	{
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા DriveBit();
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_[i];
			સ્થિતિ DriveType::WIRE:
				પાછા wire_[i];
			સ્થિતિ DriveType::PORT:
				પાછા port_[i];
			સ્થિતિ DriveType::MARKER:
				પાછા marker_[i];
			સ્થિતિ DriveType::MULTIPLE:
				પાછા multiple_[i];
		}
		log_abort();
	}

	શૂન્ય set_none(પૂર્ણાંક width = 0)
	{
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				none_ = width;
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				constant_.~Const();
				તોડો;
			સ્થિતિ DriveType::WIRE:
				wire_.~DriveChunkWire();
				તોડો;
			સ્થિતિ DriveType::PORT:
				port_.~DriveChunkPort();
				તોડો;
			સ્થિતિ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				તોડો;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk અચલ &other)
	{
		પસંદગી (other.type_)
		{
			સ્થિતિ DriveType::NONE:
				set_none(other.none_);
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				*this = other.constant_;
				તોડો;
			સ્થિતિ DriveType::WIRE:
				*this = other.wire_;
				તોડો;
			સ્થિતિ DriveType::PORT:
				*this = other.port_;
				તોડો;
			સ્થિતિ DriveType::MARKER:
				*this = other.marker_;
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				*this = other.multiple_;
				તોડો;
		}
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		પસંદગી (other.type_)
		{
			સ્થિતિ DriveType::NONE:
				set_none(other.none_);
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				તોડો;
			સ્થિતિ DriveType::WIRE:
				*this = std::move(other.wire_);
				તોડો;
			સ્થિતિ DriveType::PORT:
				*this = std::move(other.port_);
				તોડો;
			સ્થિતિ DriveType::MARKER:
				*this = std::move(other.marker_);
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				તોડો;
		}
		પાછા *this;
	}

	DriveChunk &operator=(Const અચલ &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		પાછા *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkWire અચલ &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkPort અચલ &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkMarker અચલ &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkMultiple અચલ &multiple)
	{
		set_none(multiple.size());
		જો (multiple.multiple().empty())
			પાછા *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		પાછા *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		જો (multiple.multiple().empty())
			પાછા *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		પાછા *this;
	}

	DriveChunk &operator=(DriveBit અચલ &other)
	{
		પસંદગી (other.type())
		{
			સ્થિતિ DriveType::NONE:
				set_none(1);
				તોડો;
			સ્થિતિ DriveType::CONSTANT:
				*this = Const(other.constant());
				તોડો;
			સ્થિતિ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				તોડો;
			સ્થિતિ DriveType::PORT:
				*this = DriveChunkPort(other.port());
				તોડો;
			સ્થિતિ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				તોડો;
			સ્થિતિ DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				તોડો;
		}
		પાછા *this;
	}

	bool can_append(DriveBit અચલ &bit) અચલ;
	bool try_append(DriveBit અચલ &bit);
	bool try_append(DriveChunk અચલ &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

	bool operator==(અચલ DriveChunk &other) અચલ
	{
		જો (type_ != other.type_)
			પાછા false;

		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા true;
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_ == other.constant_;
			સ્થિતિ DriveType::WIRE:
				પાછા wire_ == other.wire_;
			સ્થિતિ DriveType::PORT:
				પાછા port_ == other.port_;
			સ્થિતિ DriveType::MARKER:
				પાછા marker_ == other.marker_;
			સ્થિતિ DriveType::MULTIPLE:
				પાછા multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(અચલ DriveChunk &other) અચલ
	{
		પાછા !(*this == other);
	}

	bool operator<(અચલ DriveChunk &other) અચલ
	{
		જો (type_ != other.type_)
			પાછા type_ < other.type_;
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા false;
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_ < other.constant_;
			સ્થિતિ DriveType::WIRE:
				પાછા wire_ < other.wire_;
			સ્થિતિ DriveType::PORT:
				પાછા port_ < other.port_;
			સ્થિતિ DriveType::MARKER:
				પાછા marker_ < other.marker_;
			સ્થિતિ DriveType::MULTIPLE:
				પાછા multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() અચલ { પાછા type_; }

	bool is_none() અચલ { પાછા type_ == DriveType::NONE; }
	bool is_constant() અચલ { પાછા type_ == DriveType::CONSTANT; }
	bool is_wire() અચલ { પાછા type_ == DriveType::WIRE; }
	bool is_port() અચલ { પાછા type_ == DriveType::PORT; }
	bool is_marker() અચલ { પાછા type_ == DriveType::MARKER; }
	bool is_multiple() અચલ { પાછા type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); પાછા constant_; }
	Const અચલ &constant() અચલ { log_assert(is_constant()); પાછા constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); પાછા wire_; }
	DriveChunkWire અચલ &wire() અચલ { log_assert(is_wire()); પાછા wire_; }
	DriveChunkPort &port() { log_assert(is_port()); પાછા port_; }
	DriveChunkPort અચલ &port() અચલ { log_assert(is_port()); પાછા port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); પાછા marker_; }
	DriveChunkMarker અચલ &marker() અચલ { log_assert(is_marker()); પાછા marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); પાછા multiple_; }
	DriveChunkMultiple અચલ &multiple() અચલ { log_assert(is_multiple()); પાછા multiple_; }


	પૂર્ણાંક size() અચલ
	{
		પસંદગી (type_)
		{
			સ્થિતિ DriveType::NONE:
				પાછા none_;
			સ્થિતિ DriveType::CONSTANT:
				પાછા constant_.size();
			સ્થિતિ DriveType::WIRE:
				પાછા wire_.size();
			સ્થિતિ DriveType::PORT:
				પાછા port_.size();
			સ્થિતિ DriveType::MARKER:
				પાછા marker_.size();
			સ્થિતિ DriveType::MULTIPLE:
				પાછા multiple_.size();
		}
		log_abort();
	}
};

અંતઃરેખ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple અચલ &bit)
	: width_(1)
{
	માટે (auto અચલ &bit : bit.multiple())
		multiple_.emplace(bit);
}

સંરચના DriveSpec
{
ખાનગી:
	પૂર્ણાંક width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned પૂર્ણાંક hash_ = 0;
જાહેર:

	અંતઃરેખ bool packed() અચલ {
		પાછા bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk અચલ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire અચલ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort અચલ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker અચલ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple અચલ &chunk) { *this = chunk; }

	DriveSpec(DriveBit અચલ &bit) { *this = bit; }
	DriveSpec(DriveBitWire અચલ &bit) { *this = bit; }
	DriveSpec(DriveBitPort અચલ &bit) { *this = bit; }
	DriveSpec(DriveBitMarker અચલ &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple અચલ &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> અચલ &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> અચલ &bits)
	{
		માટે (auto અચલ &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec અચલ &sig)
	{
		// TODO: converting one chunk at a time would be faster
		માટે (auto અચલ &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> અચલ &chunks() અચલ { pack(); પાછા chunks_; }
	std::vector<DriveBit> અચલ &bits() અચલ { unpack(); પાછા bits_; }

	પૂર્ણાંક size() અચલ { પાછા width_; }

	શૂન્ય append(DriveBit અચલ &bit);

	શૂન્ય append(DriveChunk અચલ &chunk);

	શૂન્ય pack() અચલ;

	શૂન્ય unpack() અચલ;

	DriveBit &operator[](પૂર્ણાંક index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		પાછા bits_[index];
	}

	અચલ DriveBit &operator[](પૂર્ણાંક index) અચલ
	{
		log_assert(index >= 0 && index < size());
		unpack();
		પાછા bits_[index];
	}

	શૂન્ય clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk અચલ &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		પાછા *this;
	}

	DriveSpec &operator=(DriveChunkWire અચલ &chunk) { પાછા *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort અચલ &chunk) { પાછા *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker અચલ &chunk) { પાછા *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple અચલ &chunk) { પાછા *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit અચલ &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		પાછા *this;
	}

	DriveSpec &operator=(DriveBitWire અચલ &bit) { પાછા *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort અચલ &bit) { પાછા *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker અચલ &bit) { પાછા *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple અચલ &bit) { પાછા *this = DriveBit(bit); }

	શૂન્ય updhash() અચલ {
		જો (hash_ != 0)
			પાછા;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;

	bool operator==(DriveSpec અચલ &other) અચલ {
		updhash();
		other.updhash();
		જો (size() != other.size() || hash_ != other.hash_)
			પાછા false;
		પાછા chunks() == other.chunks();
	}

ખાનગી:
	શૂન્ય compute_width();
};



સંરચના DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

ખાનગી:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	સંરચના DriveBitId
	{
		પૂર્ણાંક id = -1;

		DriveBitId() {};

		DriveBitId(પૂર્ણાંક id) : id(id) { }

		bool operator==(અચલ DriveBitId &other) અચલ { પાછા id == other.id; }
		bool operator!=(અચલ DriveBitId &other) અચલ { પાછા id != other.id; }
		bool operator<(અચલ DriveBitId &other) અચલ { પાછા id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	સંરચના DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		શૂન્ય add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId કાઢો_edge(DriveBitId src);
		શૂન્ય clear(DriveBitId src);
		bool contains(DriveBitId src);
		પૂર્ણાંક ગણતરી(DriveBitId src);

		DriveBitId at(DriveBitId src, પૂર્ણાંક index);
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
	પૂર્ણાંક next_offset = 1 + (પૂર્ણાંક)State::Sm;

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


	ગણના class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit અચલ &bit);
	DriveBitId id_from_drive_bit(DriveBit અચલ &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	શૂન્ય connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	શૂન્ય connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	શૂન્ય connect_undirected(DriveBitId a_id, DriveBitId b_id);

જાહેર:

	શૂન્ય add(Module *module);

	// Add a single bit connection to the driver map.
	શૂન્ય add(DriveBit અચલ &a, DriveBit અચલ &b);

	template<typename T>
	સ્થિર constexpr bool is_sig_type() {
		પાછા
			std::is_same<T, SigSpec>::કિંમત ||
			std::is_same<T, SigChunk>::કિંમત ||
			std::is_same<T, DriveSpec>::કિંમત ||
			std::is_same<T, DriveChunk>::કિંમત ||
			std::is_same<T, DriveChunkPort>::કિંમત ||
			std::is_same<T, DriveChunkWire>::કિંમત ||
			std::is_same<T, Const>::કિંમત;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T અચલ &a, U અચલ &b)
	{
		log_assert(a.size() == b.size());
		માટે (પૂર્ણાંક i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	શૂન્ય add(SigSpec અચલ &a, SigSpec અચલ &b);

ખાનગી:
	શૂન્ય add_port(Cell *cell, IdString port, SigSpec અચલ &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, પૂર્ણાંક> orient_undirected_distance;

	શૂન્ય orient_undirected(DriveBitId id);

જાહેર:
	DriveBit operator()(DriveBit અચલ &bit);

	DriveSpec operator()(DriveSpec spec);

ખાનગી:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		પાછા wire->has_attribute(ID(keep));
	}
};

અંતઃરેખ Hasher DriveBitWire::hash_into(Hasher h) અચલ
{
	h.eat(wire->name);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveBitPort::hash_into(Hasher h) અચલ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveBitMarker::hash_into(Hasher h) અચલ
{
	h.eat(marker);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveBitMultiple::hash_into(Hasher h) અચલ
{
	h.eat(multiple_);
	પાછા h;
}

અંતઃરેખ Hasher DriveBit::hash_into(Hasher h) અચલ
{
	પસંદગી (type_) {
	સ્થિતિ DriveType::NONE:
		h.eat(0);
		તોડો;
	સ્થિતિ DriveType::CONSTANT:
		h.eat(constant_);
		તોડો;
	સ્થિતિ DriveType::WIRE:
		h.eat(wire_);
		તોડો;
	સ્થિતિ DriveType::PORT:
		h.eat(port_);
		તોડો;
	સ્થિતિ DriveType::MARKER:
		h.eat(marker_);
		તોડો;
	સ્થિતિ DriveType::MULTIPLE:
		h.eat(multiple_);
		તોડો;
	}
	h.eat(type_);
	પાછા h;
}

અંતઃરેખ Hasher DriveChunkWire::hash_into(Hasher h) અચલ
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveChunkPort::hash_into(Hasher h) અચલ
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveChunkMarker::hash_into(Hasher h) અચલ
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	પાછા h;
}

અંતઃરેખ Hasher DriveChunkMultiple::hash_into(Hasher h) અચલ
{
	h.eat(width_);
	h.eat(multiple_);
	પાછા h;
}

અંતઃરેખ Hasher DriveChunk::hash_into(Hasher h) અચલ
{
	પસંદગી (type_) {
	સ્થિતિ DriveType::NONE:
		h.eat(0);
		તોડો;
	સ્થિતિ DriveType::CONSTANT:
		h.eat(constant_);
		તોડો;
	સ્થિતિ DriveType::WIRE:
		h.eat(wire_);
		તોડો;
	સ્થિતિ DriveType::PORT:
		h.eat(port_);
		તોડો;
	સ્થિતિ DriveType::MARKER:
		h.eat(marker_);
		તોડો;
	સ્થિતિ DriveType::MULTIPLE:
		h.eat(multiple_);
		તોડો;
	}
	h.eat(type_);
	પાછા h;
}

અંતઃરેખ Hasher DriveSpec::hash_into(Hasher h) અચલ
{
	updhash();
	h.eat(hash_);
	પાછા h;
}

અંતઃરેખ Hasher DriverMap::DriveBitId::hash_into(Hasher h) અચલ
{
	h.eat(id);
	પાછા h;
}

YOSYS_NAMESPACE_END

#endif
