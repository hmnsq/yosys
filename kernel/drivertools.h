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

#ضمن <string>
#ضمن <type_traits>

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

بنية DriveBit;

بنية DriveChunkWire;
بنية DriveChunkPort;
بنية DriveChunkMarker;
بنية DriveChunk;

بنية DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(DriveChunkPort ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

تعداد class DriveType : unsigned حرف
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

بنية DriveBitWire
{
	Wire *wire;
	صحيح offset;

	DriveBitWire(Wire *wire, صحيح offset) : wire(wire), offset(offset) {}

	bool operator==(ثابت DriveBitWire &other) ثابت
	{
		أرجع wire == other.wire && offset == other.offset;
	}

	bool operator<(ثابت DriveBitWire &other) ثابت
	{
		𐡄𐡍 (wire != other.wire)
			أرجع wire->name < other.wire->name;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	operator SigBit() ثابت
	{
		أرجع SigBit(wire, offset);
	}
};

بنية DriveBitPort
{
	Cell *cell;
	IdString port;
	صحيح offset;

	DriveBitPort(Cell *cell, IdString port, صحيح offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ثابت DriveBitPort &other) ثابت
	{
		أرجع cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ثابت DriveBitPort &other) ثابت
	{
		𐡄𐡍 (cell != other.cell)
			أرجع cell->name < other.cell->name;
		𐡄𐡍 (port != other.port)
			أرجع port < other.port;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


بنية DriveBitMarker
{
	صحيح marker;
	صحيح offset;

	DriveBitMarker(صحيح marker, صحيح offset) : marker(marker), offset(offset) {}

	bool operator==(ثابت DriveBitMarker &other) ثابت
	{
		أرجع marker == other.marker && offset == other.offset;
	}

	bool operator<(ثابت DriveBitMarker &other) ثابت
	{
		𐡄𐡍 (marker != other.marker)
			أرجع marker < other.marker;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

بنية DriveBitMultiple
{
خاص:
	pool<DriveBit> multiple_;

عام:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ثابت &single);

	pool<DriveBit> ثابت &multiple() ثابت { أرجع multiple_; }

	فراغ merge(DriveBitMultiple ثابت &other)
	{
		لكل (DriveBit ثابت &single : other.multiple_)
			merge(single);
	}

	فراغ merge(DriveBitMultiple &&other)
	{
		لكل (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	فراغ merge(DriveBit ثابت &single);
	فراغ merge(DriveBit &&single);

	bool operator==(ثابت DriveBitMultiple &other) ثابت
	{
		أرجع multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية DriveBit
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
عام:
	DriveBit()  {}

	DriveBit(SigBit ثابت &bit);

	DriveBit(DriveBit ثابت &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ثابت &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ثابت &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ثابت &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ثابت &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	فراغ set_none()
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				اكسر;
			حالة DriveType::CONSTANT:
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveBitWire();
				اكسر;
			حالة DriveType::PORT:
				port_.~DriveBitPort();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveBitMarker();
				اكسر;
			حالة DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				اكسر;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				set_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::PORT:
				*this = other.port_;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::MULTIPLE:
				*this = other.multiple_;
				اكسر;
		}
		أرجع *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				set_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::PORT:
				*this = std::move(other.port_);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				اكسر;
		}
		أرجع *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitWire ثابت &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitPort ثابت &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitMarker ثابت &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitMultiple ثابت &multiple)
	{
		set_none();
		𐡄𐡍 (multiple.multiple().empty())
			أرجع *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		أرجع *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		𐡄𐡍 (multiple.multiple().empty())
			أرجع *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		أرجع *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(ثابت DriveBit &other) ثابت
	{
		𐡄𐡍 (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::PORT:
				أرجع port_ == other.port_;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::MULTIPLE:
				أرجع multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ثابت DriveBit &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool operator<(ثابت DriveBit &other) ثابت
	{
		𐡄𐡍 (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::PORT:
				أرجع port_ < other.port_;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_port() ثابت { أرجع type_ == DriveType::PORT; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_multiple() ثابت { أرجع type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); أرجع constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveBitWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	DriveBitPort &port() { log_assert(is_port()); أرجع port_; }
	DriveBitPort ثابت &port() ثابت { log_assert(is_port()); أرجع port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); أرجع marker_; }
	DriveBitMarker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); أرجع multiple_; }
	DriveBitMultiple ثابت &multiple() ثابت { log_assert(is_multiple()); أرجع multiple_; }

	فراغ merge(DriveBit ثابت &other);

};

مضمن DriveBitMultiple::DriveBitMultiple() {}
مضمن DriveBitMultiple::DriveBitMultiple(DriveBit ثابت &single)
{
	multiple_.emplace(single);
}

بنية DriveChunkWire
{
	Wire *wire;
	صحيح offset;
	صحيح width;

	DriveChunkWire(Wire *wire, صحيح offset, صحيح width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ثابت &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	صحيح size() ثابت { أرجع width; }

	DriveBitWire operator[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		أرجع DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ثابت &bit) ثابت;
	bool try_append(DriveBitWire ثابت &bit);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ثابت { أرجع offset == 0 && width == wire->width; }

	bool operator==(ثابت DriveChunkWire &other) ثابت
	{
		أرجع wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkWire &other) ثابت
	{
		𐡄𐡍 (wire != other.wire)
			أرجع wire->name < other.wire->name;
		𐡄𐡍 (width != other.width)
			أرجع width < other.width;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit operator SigChunk() ثابت
	{
		أرجع SigChunk(wire, offset, width);
	}
};

بنية DriveChunkPort
{
	Cell *cell;
	IdString port;
	صحيح offset;
	صحيح width;

	DriveChunkPort(Cell *cell, IdString port, صحيح offset, صحيح width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ثابت &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	صحيح size() ثابت { أرجع width; }

	DriveBitPort operator[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		أرجع DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ثابت &bit) ثابت;
	bool try_append(DriveBitPort ثابت &bit);
	bool try_append(DriveChunkPort ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ثابت
	{
		أرجع offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ثابت DriveChunkPort &other) ثابت
	{
		أرجع cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkPort &other) ثابت
	{
		𐡄𐡍 (cell != other.cell)
			أرجع cell->name < other.cell->name;
		𐡄𐡍 (port != other.port)
			أرجع port < other.port;
		𐡄𐡍 (width != other.width)
			أرجع width < other.width;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


بنية DriveChunkMarker
{
	صحيح marker;
	صحيح offset;
	صحيح width;

	DriveChunkMarker(صحيح marker, صحيح offset, صحيح width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ثابت &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	صحيح size() ثابت { أرجع width; }

	DriveBitMarker operator[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < width);
		أرجع DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ثابت &bit) ثابت;
	bool try_append(DriveBitMarker ثابت &bit);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool operator==(ثابت DriveChunkMarker &other) ثابت
	{
		أرجع marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ثابت DriveChunkMarker &other) ثابت
	{
		𐡄𐡍 (marker != other.marker)
			أرجع marker < other.marker;
		𐡄𐡍 (width != other.width)
			أرجع width < other.width;
		أرجع offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية DriveChunkMultiple
{
خاص:
	mutable pool<DriveChunk> multiple_;
	صحيح width_;

عام:
	pool<DriveChunk> ثابت &multiple() ثابت { أرجع multiple_; }

	DriveChunkMultiple(DriveBitMultiple ثابت &bit);

	صحيح size() ثابت { أرجع width_; }

	DriveBitMultiple operator[](صحيح i) ثابت;

	bool can_append(DriveBitMultiple ثابت &bit) ثابت;

	bool try_append(DriveBitMultiple ثابت &bit);


	bool can_append(DriveChunkMultiple ثابت &bit) ثابت;

	bool try_append(DriveChunkMultiple ثابت &bit);

	bool operator==(ثابت DriveChunkMultiple &other) ثابت
	{
		أرجع width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ثابت DriveChunkMultiple &other) ثابت
	{
		𐡄𐡍 (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		أرجع false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية DriveChunk
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		صحيح none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

عام:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ثابت &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ثابت &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](صحيح i) ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع DriveBit();
			حالة DriveType::CONSTANT:
				أرجع constant_[i];
			حالة DriveType::WIRE:
				أرجع wire_[i];
			حالة DriveType::PORT:
				أرجع port_[i];
			حالة DriveType::MARKER:
				أرجع marker_[i];
			حالة DriveType::MULTIPLE:
				أرجع multiple_[i];
		}
		log_abort();
	}

	فراغ set_none(صحيح width = 0)
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				none_ = width;
				اكسر;
			حالة DriveType::CONSTANT:
				constant_.~Const();
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveChunkWire();
				اكسر;
			حالة DriveType::PORT:
				port_.~DriveChunkPort();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveChunkMarker();
				اكسر;
			حالة DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				اكسر;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				set_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::PORT:
				*this = other.port_;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::MULTIPLE:
				*this = other.multiple_;
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				set_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::PORT:
				*this = std::move(other.port_);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &operator=(Const ثابت &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkWire ثابت &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkPort ثابت &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkMarker ثابت &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ثابت &multiple)
	{
		set_none(multiple.size());
		𐡄𐡍 (multiple.multiple().empty())
			أرجع *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		أرجع *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		𐡄𐡍 (multiple.multiple().empty())
			أرجع *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		أرجع *this;
	}

	DriveChunk &operator=(DriveBit ثابت &other)
	{
		بدل (other.type())
		{
			حالة DriveType::NONE:
				set_none(1);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = Const(other.constant());
				اكسر;
			حالة DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				اكسر;
			حالة DriveType::PORT:
				*this = DriveChunkPort(other.port());
				اكسر;
			حالة DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				اكسر;
			حالة DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				اكسر;
		}
		أرجع *this;
	}

	bool can_append(DriveBit ثابت &bit) ثابت;
	bool try_append(DriveBit ثابت &bit);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(ثابت DriveChunk &other) ثابت
	{
		𐡄𐡍 (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::PORT:
				أرجع port_ == other.port_;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::MULTIPLE:
				أرجع multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ثابت DriveChunk &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool operator<(ثابت DriveChunk &other) ثابت
	{
		𐡄𐡍 (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::PORT:
				أرجع port_ < other.port_;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::MULTIPLE:
				أرجع multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_port() ثابت { أرجع type_ == DriveType::PORT; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_multiple() ثابت { أرجع type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); أرجع constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	DriveChunkPort &port() { log_assert(is_port()); أرجع port_; }
	DriveChunkPort ثابت &port() ثابت { log_assert(is_port()); أرجع port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); أرجع marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); أرجع multiple_; }
	DriveChunkMultiple ثابت &multiple() ثابت { log_assert(is_multiple()); أرجع multiple_; }


	صحيح size() ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع none_;
			حالة DriveType::CONSTANT:
				أرجع constant_.size();
			حالة DriveType::WIRE:
				أرجع wire_.size();
			حالة DriveType::PORT:
				أرجع port_.size();
			حالة DriveType::MARKER:
				أرجع marker_.size();
			حالة DriveType::MULTIPLE:
				أرجع multiple_.size();
		}
		log_abort();
	}
};

مضمن DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ثابت &bit)
	: width_(1)
{
	لكل (auto ثابت &bit : bit.multiple())
		multiple_.emplace(bit);
}

بنية DriveSpec
{
خاص:
	صحيح width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned صحيح hash_ = 0;
عام:

	مضمن bool packed() ثابت {
		أرجع bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ثابت &chunk) { *this = chunk; }

	DriveSpec(DriveBit ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitWire ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitPort ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ثابت &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ثابت &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ثابت &bits)
	{
		لكل (auto ثابت &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		لكل (auto ثابت &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ثابت &chunks() ثابت { pack(); أرجع chunks_; }
	std::vector<DriveBit> ثابت &bits() ثابت { unpack(); أرجع bits_; }

	صحيح size() ثابت { أرجع width_; }

	فراغ append(DriveBit ثابت &bit);

	فراغ append(DriveChunk ثابت &chunk);

	فراغ pack() ثابت;

	فراغ unpack() ثابت;

	DriveBit &operator[](صحيح index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		أرجع bits_[index];
	}

	ثابت DriveBit &operator[](صحيح index) ثابت
	{
		log_assert(index >= 0 && index < size());
		unpack();
		أرجع bits_[index];
	}

	فراغ clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		أرجع *this;
	}

	DriveSpec &operator=(DriveChunkWire ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ثابت &chunk) { أرجع *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ثابت &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		أرجع *this;
	}

	DriveSpec &operator=(DriveBitWire ثابت &bit) { أرجع *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ثابت &bit) { أرجع *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ثابت &bit) { أرجع *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ثابت &bit) { أرجع *this = DriveBit(bit); }

	فراغ updhash() ثابت {
		𐡄𐡍 (hash_ != 0)
			أرجع;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool operator==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		𐡄𐡍 (size() != other.size() || hash_ != other.hash_)
			أرجع false;
		أرجع chunks() == other.chunks();
	}

خاص:
	فراغ compute_width();
};



بنية DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

خاص:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	بنية DriveBitId
	{
		صحيح id = -1;

		DriveBitId() {};

		DriveBitId(صحيح id) : id(id) { }

		bool operator==(ثابت DriveBitId &other) ثابت { أرجع id == other.id; }
		bool operator!=(ثابت DriveBitId &other) ثابت { أرجع id != other.id; }
		bool operator<(ثابت DriveBitId &other) ثابت { أرجع id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	بنية DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		فراغ add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId pop_edge(DriveBitId src);
		فراغ clear(DriveBitId src);
		bool contains(DriveBitId src);
		صحيح count(DriveBitId src);

		DriveBitId at(DriveBitId src, صحيح index);
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
	صحيح next_offset = 1 + (صحيح)State::Sm;

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


	تعداد class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit ثابت &bit);
	DriveBitId id_from_drive_bit(DriveBit ثابت &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	فراغ connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	فراغ connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	فراغ connect_undirected(DriveBitId a_id, DriveBitId b_id);

عام:

	فراغ add(Module *module);

	// Add a single bit connection to the driver map.
	فراغ add(DriveBit ثابت &a, DriveBit ثابت &b);

	template<typename T>
	ساكن constexpr bool is_sig_type() {
		أرجع
			std::is_same<T, SigSpec>::value ||
			std::is_same<T, SigChunk>::value ||
			std::is_same<T, DriveSpec>::value ||
			std::is_same<T, DriveChunk>::value ||
			std::is_same<T, DriveChunkPort>::value ||
			std::is_same<T, DriveChunkWire>::value ||
			std::is_same<T, Const>::value;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ثابت &a, U ثابت &b)
	{
		log_assert(a.size() == b.size());
		لكل (صحيح i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	فراغ add(SigSpec ثابت &a, SigSpec ثابت &b);

خاص:
	فراغ add_port(Cell *cell, IdString port, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, صحيح> orient_undirected_distance;

	فراغ orient_undirected(DriveBitId id);

عام:
	DriveBit operator()(DriveBit ثابت &bit);

	DriveSpec operator()(DriveSpec spec);

خاص:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		أرجع wire->has_attribute(ID(keep));
	}
};

مضمن Hasher DriveBitWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveBitPort::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveBitMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveBitMultiple::hash_into(Hasher h) ثابت
{
	h.eat(multiple_);
	أرجع h;
}

مضمن Hasher DriveBit::hash_into(Hasher h) ثابت
{
	بدل (type_) {
	حالة DriveType::NONE:
		h.eat(0);
		اكسر;
	حالة DriveType::CONSTANT:
		h.eat(constant_);
		اكسر;
	حالة DriveType::WIRE:
		h.eat(wire_);
		اكسر;
	حالة DriveType::PORT:
		h.eat(port_);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::MULTIPLE:
		h.eat(multiple_);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveChunkPort::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	أرجع h;
}

مضمن Hasher DriveChunkMultiple::hash_into(Hasher h) ثابت
{
	h.eat(width_);
	h.eat(multiple_);
	أرجع h;
}

مضمن Hasher DriveChunk::hash_into(Hasher h) ثابت
{
	بدل (type_) {
	حالة DriveType::NONE:
		h.eat(0);
		اكسر;
	حالة DriveType::CONSTANT:
		h.eat(constant_);
		اكسر;
	حالة DriveType::WIRE:
		h.eat(wire_);
		اكسر;
	حالة DriveType::PORT:
		h.eat(port_);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::MULTIPLE:
		h.eat(multiple_);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveSpec::hash_into(Hasher h) ثابت
{
	updhash();
	h.eat(hash_);
	أرجع h;
}

مضمن Hasher DriverMap::DriveBitId::hash_into(Hasher h) ثابت
{
	h.eat(id);
	أرجع h;
}

YOSYS_NAMESPACE_END

#endif
