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

#अन्तर्भाव <string>
#अन्तर्भाव <type_traits>

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

संरचना DriveBit;

संरचना DriveChunkWire;
संरचना DriveChunkPort;
संरचना DriveChunkMarker;
संरचना DriveChunk;

संरचना DriveSpec;

std::string log_signal(DriveChunkWire अचर &chunk);
std::string log_signal(DriveChunkPort अचर &chunk);
std::string log_signal(DriveChunkMarker अचर &chunk);
std::string log_signal(DriveChunk अचर &chunk);
std::string log_signal(DriveSpec अचर &chunk);

गणना class DriveType : अचिन्हित वर्ण
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

संरचना DriveBitWire
{
	Wire *wire;
	पूर्णाङ्क offset;

	DriveBitWire(Wire *wire, पूर्णाङ्क offset) : wire(wire), offset(offset) {}

	bool चालक==(अचर DriveBitWire &other) अचर
	{
		फर्काऊ wire == other.wire && offset == other.offset;
	}

	bool चालक<(अचर DriveBitWire &other) अचर
	{
		यदि (wire != other.wire)
			फर्काऊ wire->name < other.wire->name;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;


	चालक SigBit() अचर
	{
		फर्काऊ SigBit(wire, offset);
	}
};

संरचना DriveBitPort
{
	Cell *cell;
	IdString port;
	पूर्णाङ्क offset;

	DriveBitPort(Cell *cell, IdString port, पूर्णाङ्क offset) : cell(cell), port(port), offset(offset) {}

	bool चालक==(अचर DriveBitPort &other) अचर
	{
		फर्काऊ cell == other.cell && port == other.port && offset == other.offset;
	}

	bool चालक<(अचर DriveBitPort &other) अचर
	{
		यदि (cell != other.cell)
			फर्काऊ cell->name < other.cell->name;
		यदि (port != other.port)
			फर्काऊ port < other.port;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

};


संरचना DriveBitMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क offset;

	DriveBitMarker(पूर्णाङ्क marker, पूर्णाङ्क offset) : marker(marker), offset(offset) {}

	bool चालक==(अचर DriveBitMarker &other) अचर
	{
		फर्काऊ marker == other.marker && offset == other.offset;
	}

	bool चालक<(अचर DriveBitMarker &other) अचर
	{
		यदि (marker != other.marker)
			फर्काऊ marker < other.marker;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

};

संरचना DriveBitMultiple
{
गुप्त:
	pool<DriveBit> multiple_;

सार्वजनिक:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit अचर &single);

	pool<DriveBit> अचर &multiple() अचर { फर्काऊ multiple_; }

	शून्य merge(DriveBitMultiple अचर &other)
	{
		लागि (DriveBit अचर &single : other.multiple_)
			merge(single);
	}

	शून्य merge(DriveBitMultiple &&other)
	{
		लागि (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	शून्य merge(DriveBit अचर &single);
	शून्य merge(DriveBit &&single);

	bool चालक==(अचर DriveBitMultiple &other) अचर
	{
		फर्काऊ multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
};

संरचना DriveBit
{
गुप्त:
	DriveType type_ = DriveType::NONE;
	सङ्घ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
सार्वजनिक:
	DriveBit()  {}

	DriveBit(SigBit अचर &bit);

	DriveBit(DriveBit अचर &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire अचर &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort अचर &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker अचर &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple अचर &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	शून्य set_none()
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				रोक;
			पक्ष DriveType::CONSTANT:
				रोक;
			पक्ष DriveType::WIRE:
				wire_.~DriveBitWire();
				रोक;
			पक्ष DriveType::PORT:
				port_.~DriveBitPort();
				रोक;
			पक्ष DriveType::MARKER:
				marker_.~DriveBitMarker();
				रोक;
			पक्ष DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				रोक;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &चालक=(DriveBit अचर &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none();
				रोक;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				रोक;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				रोक;
			पक्ष DriveType::PORT:
				*this = other.port_;
				रोक;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				रोक;
			पक्ष DriveType::MULTIPLE:
				*this = other.multiple_;
				रोक;
		}
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBit &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none();
				रोक;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				रोक;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				रोक;
			पक्ष DriveType::PORT:
				*this = std::move(other.port_);
				रोक;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				रोक;
			पक्ष DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				रोक;
		}
		फर्काऊ *this;
	}

	DriveBit &चालक=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitWire अचर &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitPort अचर &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitMarker अचर &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitMultiple अचर &multiple)
	{
		set_none();
		यदि (multiple.multiple().empty())
			फर्काऊ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		फर्काऊ *this;
	}

	DriveBit &चालक=(DriveBitMultiple &&multiple)
	{
		set_none();
		यदि (multiple.multiple().empty())
			फर्काऊ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		फर्काऊ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

	bool चालक==(अचर DriveBit &other) अचर
	{
		यदि (type_ != other.type_)
			फर्काऊ false;

		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ true;
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_ == other.constant_;
			पक्ष DriveType::WIRE:
				फर्काऊ wire_ == other.wire_;
			पक्ष DriveType::PORT:
				फर्काऊ port_ == other.port_;
			पक्ष DriveType::MARKER:
				फर्काऊ marker_ == other.marker_;
			पक्ष DriveType::MULTIPLE:
				फर्काऊ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(अचर DriveBit &other) अचर
	{
		फर्काऊ !(*this == other);
	}

	bool चालक<(अचर DriveBit &other) अचर
	{
		यदि (type_ != other.type_)
			फर्काऊ type_ < other.type_;
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ false;
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_ < other.constant_;
			पक्ष DriveType::WIRE:
				फर्काऊ wire_ < other.wire_;
			पक्ष DriveType::PORT:
				फर्काऊ port_ < other.port_;
			पक्ष DriveType::MARKER:
				फर्काऊ marker_ < other.marker_;
			पक्ष DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() अचर { फर्काऊ type_; }

	bool is_none() अचर { फर्काऊ type_ == DriveType::NONE; }
	bool is_constant() अचर { फर्काऊ type_ == DriveType::CONSTANT; }
	bool is_wire() अचर { फर्काऊ type_ == DriveType::WIRE; }
	bool is_port() अचर { फर्काऊ type_ == DriveType::PORT; }
	bool is_marker() अचर { फर्काऊ type_ == DriveType::MARKER; }
	bool is_multiple() अचर { फर्काऊ type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); फर्काऊ constant_; }
	State अचर &constant() अचर { log_assert(is_constant()); फर्काऊ constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); फर्काऊ wire_; }
	DriveBitWire अचर &wire() अचर { log_assert(is_wire()); फर्काऊ wire_; }
	DriveBitPort &port() { log_assert(is_port()); फर्काऊ port_; }
	DriveBitPort अचर &port() अचर { log_assert(is_port()); फर्काऊ port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); फर्काऊ marker_; }
	DriveBitMarker अचर &marker() अचर { log_assert(is_marker()); फर्काऊ marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); फर्काऊ multiple_; }
	DriveBitMultiple अचर &multiple() अचर { log_assert(is_multiple()); फर्काऊ multiple_; }

	शून्य merge(DriveBit अचर &other);

};

अन्तःस्थ DriveBitMultiple::DriveBitMultiple() {}
अन्तःस्थ DriveBitMultiple::DriveBitMultiple(DriveBit अचर &single)
{
	multiple_.emplace(single);
}

संरचना DriveChunkWire
{
	Wire *wire;
	पूर्णाङ्क offset;
	पूर्णाङ्क width;

	DriveChunkWire(Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire अचर &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	पूर्णाङ्क size() अचर { फर्काऊ width; }

	DriveBitWire चालक[](पूर्णाङ्क i) अचर
	{
		log_assert(i >= 0 && i < width);
		फर्काऊ DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire अचर &bit) अचर;
	bool try_append(DriveBitWire अचर &bit);
	bool try_append(DriveChunkWire अचर &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() अचर { फर्काऊ offset == 0 && width == wire->width; }

	bool चालक==(अचर DriveChunkWire &other) अचर
	{
		फर्काऊ wire == other.wire && offset == other.offset && width == other.width;
	}

	bool चालक<(अचर DriveChunkWire &other) अचर
	{
		यदि (wire != other.wire)
			फर्काऊ wire->name < other.wire->name;
		यदि (width != other.width)
			फर्काऊ width < other.width;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

	explicit चालक SigChunk() अचर
	{
		फर्काऊ SigChunk(wire, offset, width);
	}
};

संरचना DriveChunkPort
{
	Cell *cell;
	IdString port;
	पूर्णाङ्क offset;
	पूर्णाङ्क width;

	DriveChunkPort(Cell *cell, IdString port, पूर्णाङ्क offset, पूर्णाङ्क width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> अचर &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort अचर &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	पूर्णाङ्क size() अचर { फर्काऊ width; }

	DriveBitPort चालक[](पूर्णाङ्क i) अचर
	{
		log_assert(i >= 0 && i < width);
		फर्काऊ DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort अचर &bit) अचर;
	bool try_append(DriveBitPort अचर &bit);
	bool try_append(DriveChunkPort अचर &chunk);

	// Whether this chunk is a whole port
	bool is_whole() अचर
	{
		फर्काऊ offset == 0 && width == cell->connections().at(port).size();
	}

	bool चालक==(अचर DriveChunkPort &other) अचर
	{
		फर्काऊ cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool चालक<(अचर DriveChunkPort &other) अचर
	{
		यदि (cell != other.cell)
			फर्काऊ cell->name < other.cell->name;
		यदि (port != other.port)
			फर्काऊ port < other.port;
		यदि (width != other.width)
			फर्काऊ width < other.width;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
};


संरचना DriveChunkMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क offset;
	पूर्णाङ्क width;

	DriveChunkMarker(पूर्णाङ्क marker, पूर्णाङ्क offset, पूर्णाङ्क width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker अचर &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	पूर्णाङ्क size() अचर { फर्काऊ width; }

	DriveBitMarker चालक[](पूर्णाङ्क i) अचर
	{
		log_assert(i >= 0 && i < width);
		फर्काऊ DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker अचर &bit) अचर;
	bool try_append(DriveBitMarker अचर &bit);
	bool try_append(DriveChunkMarker अचर &chunk);

	bool चालक==(अचर DriveChunkMarker &other) अचर
	{
		फर्काऊ marker == other.marker && offset == other.offset && width == other.width;
	}

	bool चालक<(अचर DriveChunkMarker &other) अचर
	{
		यदि (marker != other.marker)
			फर्काऊ marker < other.marker;
		यदि (width != other.width)
			फर्काऊ width < other.width;
		फर्काऊ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
};

संरचना DriveChunkMultiple
{
गुप्त:
	mutable pool<DriveChunk> multiple_;
	पूर्णाङ्क width_;

सार्वजनिक:
	pool<DriveChunk> अचर &multiple() अचर { फर्काऊ multiple_; }

	DriveChunkMultiple(DriveBitMultiple अचर &bit);

	पूर्णाङ्क size() अचर { फर्काऊ width_; }

	DriveBitMultiple चालक[](पूर्णाङ्क i) अचर;

	bool can_append(DriveBitMultiple अचर &bit) अचर;

	bool try_append(DriveBitMultiple अचर &bit);


	bool can_append(DriveChunkMultiple अचर &bit) अचर;

	bool try_append(DriveChunkMultiple अचर &bit);

	bool चालक==(अचर DriveChunkMultiple &other) अचर
	{
		फर्काऊ width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool चालक<(अचर DriveChunkMultiple &other) अचर
	{
		यदि (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		फर्काऊ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;
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
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

सार्वजनिक:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk अचर &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit अचर &other) { *this = other; }

	DriveChunk(Const अचर &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire अचर &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort अचर &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker अचर &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple अचर &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit चालक[](पूर्णाङ्क i) अचर
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ DriveBit();
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_[i];
			पक्ष DriveType::WIRE:
				फर्काऊ wire_[i];
			पक्ष DriveType::PORT:
				फर्काऊ port_[i];
			पक्ष DriveType::MARKER:
				फर्काऊ marker_[i];
			पक्ष DriveType::MULTIPLE:
				फर्काऊ multiple_[i];
		}
		log_abort();
	}

	शून्य set_none(पूर्णाङ्क width = 0)
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				none_ = width;
				रोक;
			पक्ष DriveType::CONSTANT:
				constant_.~Const();
				रोक;
			पक्ष DriveType::WIRE:
				wire_.~DriveChunkWire();
				रोक;
			पक्ष DriveType::PORT:
				port_.~DriveChunkPort();
				रोक;
			पक्ष DriveType::MARKER:
				marker_.~DriveChunkMarker();
				रोक;
			पक्ष DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				रोक;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &चालक=(DriveChunk अचर &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none(other.none_);
				रोक;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				रोक;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				रोक;
			पक्ष DriveType::PORT:
				*this = other.port_;
				रोक;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				रोक;
			पक्ष DriveType::MULTIPLE:
				*this = other.multiple_;
				रोक;
		}
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunk &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none(other.none_);
				रोक;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				रोक;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				रोक;
			पक्ष DriveType::PORT:
				*this = std::move(other.port_);
				रोक;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				रोक;
			पक्ष DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				रोक;
		}
		फर्काऊ *this;
	}

	DriveChunk &चालक=(Const अचर &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkWire अचर &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkPort अचर &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkMarker अचर &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple अचर &multiple)
	{
		set_none(multiple.size());
		यदि (multiple.multiple().empty())
			फर्काऊ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		यदि (multiple.multiple().empty())
			फर्काऊ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		फर्काऊ *this;
	}

	DriveChunk &चालक=(DriveBit अचर &other)
	{
		विकल्प (other.type())
		{
			पक्ष DriveType::NONE:
				set_none(1);
				रोक;
			पक्ष DriveType::CONSTANT:
				*this = Const(other.constant());
				रोक;
			पक्ष DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				रोक;
			पक्ष DriveType::PORT:
				*this = DriveChunkPort(other.port());
				रोक;
			पक्ष DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				रोक;
			पक्ष DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				रोक;
		}
		फर्काऊ *this;
	}

	bool can_append(DriveBit अचर &bit) अचर;
	bool try_append(DriveBit अचर &bit);
	bool try_append(DriveChunk अचर &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

	bool चालक==(अचर DriveChunk &other) अचर
	{
		यदि (type_ != other.type_)
			फर्काऊ false;

		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ true;
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_ == other.constant_;
			पक्ष DriveType::WIRE:
				फर्काऊ wire_ == other.wire_;
			पक्ष DriveType::PORT:
				फर्काऊ port_ == other.port_;
			पक्ष DriveType::MARKER:
				फर्काऊ marker_ == other.marker_;
			पक्ष DriveType::MULTIPLE:
				फर्काऊ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(अचर DriveChunk &other) अचर
	{
		फर्काऊ !(*this == other);
	}

	bool चालक<(अचर DriveChunk &other) अचर
	{
		यदि (type_ != other.type_)
			फर्काऊ type_ < other.type_;
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ false;
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_ < other.constant_;
			पक्ष DriveType::WIRE:
				फर्काऊ wire_ < other.wire_;
			पक्ष DriveType::PORT:
				फर्काऊ port_ < other.port_;
			पक्ष DriveType::MARKER:
				फर्काऊ marker_ < other.marker_;
			पक्ष DriveType::MULTIPLE:
				फर्काऊ multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() अचर { फर्काऊ type_; }

	bool is_none() अचर { फर्काऊ type_ == DriveType::NONE; }
	bool is_constant() अचर { फर्काऊ type_ == DriveType::CONSTANT; }
	bool is_wire() अचर { फर्काऊ type_ == DriveType::WIRE; }
	bool is_port() अचर { फर्काऊ type_ == DriveType::PORT; }
	bool is_marker() अचर { फर्काऊ type_ == DriveType::MARKER; }
	bool is_multiple() अचर { फर्काऊ type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); फर्काऊ constant_; }
	Const अचर &constant() अचर { log_assert(is_constant()); फर्काऊ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); फर्काऊ wire_; }
	DriveChunkWire अचर &wire() अचर { log_assert(is_wire()); फर्काऊ wire_; }
	DriveChunkPort &port() { log_assert(is_port()); फर्काऊ port_; }
	DriveChunkPort अचर &port() अचर { log_assert(is_port()); फर्काऊ port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); फर्काऊ marker_; }
	DriveChunkMarker अचर &marker() अचर { log_assert(is_marker()); फर्काऊ marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); फर्काऊ multiple_; }
	DriveChunkMultiple अचर &multiple() अचर { log_assert(is_multiple()); फर्काऊ multiple_; }


	पूर्णाङ्क size() अचर
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				फर्काऊ none_;
			पक्ष DriveType::CONSTANT:
				फर्काऊ constant_.size();
			पक्ष DriveType::WIRE:
				फर्काऊ wire_.size();
			पक्ष DriveType::PORT:
				फर्काऊ port_.size();
			पक्ष DriveType::MARKER:
				फर्काऊ marker_.size();
			पक्ष DriveType::MULTIPLE:
				फर्काऊ multiple_.size();
		}
		log_abort();
	}
};

अन्तःस्थ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple अचर &bit)
	: width_(1)
{
	लागि (स्वतः अचर &bit : bit.multiple())
		multiple_.emplace(bit);
}

संरचना DriveSpec
{
गुप्त:
	पूर्णाङ्क width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable अचिन्हित पूर्णाङ्क hash_ = 0;
सार्वजनिक:

	अन्तःस्थ bool packed() अचर {
		फर्काऊ bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk अचर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire अचर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort अचर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker अचर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple अचर &chunk) { *this = chunk; }

	DriveSpec(DriveBit अचर &bit) { *this = bit; }
	DriveSpec(DriveBitWire अचर &bit) { *this = bit; }
	DriveSpec(DriveBitPort अचर &bit) { *this = bit; }
	DriveSpec(DriveBitMarker अचर &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple अचर &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> अचर &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> अचर &bits)
	{
		लागि (स्वतः अचर &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec अचर &sig)
	{
		// TODO: converting one chunk at a time would be faster
		लागि (स्वतः अचर &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> अचर &chunks() अचर { pack(); फर्काऊ chunks_; }
	std::vector<DriveBit> अचर &bits() अचर { unpack(); फर्काऊ bits_; }

	पूर्णाङ्क size() अचर { फर्काऊ width_; }

	शून्य append(DriveBit अचर &bit);

	शून्य append(DriveChunk अचर &chunk);

	शून्य pack() अचर;

	शून्य unpack() अचर;

	DriveBit &चालक[](पूर्णाङ्क index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		फर्काऊ bits_[index];
	}

	अचर DriveBit &चालक[](पूर्णाङ्क index) अचर
	{
		log_assert(index >= 0 && index < size());
		unpack();
		फर्काऊ bits_[index];
	}

	शून्य clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &चालक=(DriveChunk अचर &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		फर्काऊ *this;
	}

	DriveSpec &चालक=(DriveChunkWire अचर &chunk) { फर्काऊ *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkPort अचर &chunk) { फर्काऊ *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMarker अचर &chunk) { फर्काऊ *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMultiple अचर &chunk) { फर्काऊ *this = DriveChunk(chunk); }

	DriveSpec &चालक=(DriveBit अचर &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		फर्काऊ *this;
	}

	DriveSpec &चालक=(DriveBitWire अचर &bit) { फर्काऊ *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitPort अचर &bit) { फर्काऊ *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMarker अचर &bit) { फर्काऊ *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMultiple अचर &bit) { फर्काऊ *this = DriveBit(bit); }

	शून्य updhash() अचर {
		यदि (hash_ != 0)
			फर्काऊ;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) अचर;

	bool चालक==(DriveSpec अचर &other) अचर {
		updhash();
		other.updhash();
		यदि (size() != other.size() || hash_ != other.hash_)
			फर्काऊ false;
		फर्काऊ chunks() == other.chunks();
	}

गुप्त:
	शून्य compute_width();
};



संरचना DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

गुप्त:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	संरचना DriveBitId
	{
		पूर्णाङ्क id = -1;

		DriveBitId() {};

		DriveBitId(पूर्णाङ्क id) : id(id) { }

		bool चालक==(अचर DriveBitId &other) अचर { फर्काऊ id == other.id; }
		bool चालक!=(अचर DriveBitId &other) अचर { फर्काऊ id != other.id; }
		bool चालक<(अचर DriveBitId &other) अचर { फर्काऊ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) अचर;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	संरचना DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		शून्य add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId झिक_edge(DriveBitId src);
		शून्य clear(DriveBitId src);
		bool contains(DriveBitId src);
		पूर्णाङ्क गन्ती(DriveBitId src);

		DriveBitId at(DriveBitId src, पूर्णाङ्क index);
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
	पूर्णाङ्क next_offset = 1 + (पूर्णाङ्क)State::Sm;

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


	गणना class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit अचर &bit);
	DriveBitId id_from_drive_bit(DriveBit अचर &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	शून्य connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_undirected(DriveBitId a_id, DriveBitId b_id);

सार्वजनिक:

	शून्य add(Module *module);

	// Add a single bit connection to the driver map.
	शून्य add(DriveBit अचर &a, DriveBit अचर &b);

	ढाँचा<typename T>
	स्थिर constexpr bool is_sig_type() {
		फर्काऊ
			std::is_same<T, SigSpec>::मान ||
			std::is_same<T, SigChunk>::मान ||
			std::is_same<T, DriveSpec>::मान ||
			std::is_same<T, DriveChunk>::मान ||
			std::is_same<T, DriveChunkPort>::मान ||
			std::is_same<T, DriveChunkWire>::मान ||
			std::is_same<T, Const>::मान;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	ढाँचा<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T अचर &a, U अचर &b)
	{
		log_assert(a.size() == b.size());
		लागि (पूर्णाङ्क i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	शून्य add(SigSpec अचर &a, SigSpec अचर &b);

गुप्त:
	शून्य add_port(Cell *cell, IdString port, SigSpec अचर &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, पूर्णाङ्क> orient_undirected_distance;

	शून्य orient_undirected(DriveBitId id);

सार्वजनिक:
	DriveBit चालक()(DriveBit अचर &bit);

	DriveSpec चालक()(DriveSpec spec);

गुप्त:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		फर्काऊ wire->has_attribute(ID(keep));
	}
};

अन्तःस्थ Hasher DriveBitWire::hash_into(Hasher h) अचर
{
	h.eat(wire->name);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveBitPort::hash_into(Hasher h) अचर
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveBitMarker::hash_into(Hasher h) अचर
{
	h.eat(marker);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveBitMultiple::hash_into(Hasher h) अचर
{
	h.eat(multiple_);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveBit::hash_into(Hasher h) अचर
{
	विकल्प (type_) {
	पक्ष DriveType::NONE:
		h.eat(0);
		रोक;
	पक्ष DriveType::CONSTANT:
		h.eat(constant_);
		रोक;
	पक्ष DriveType::WIRE:
		h.eat(wire_);
		रोक;
	पक्ष DriveType::PORT:
		h.eat(port_);
		रोक;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		रोक;
	पक्ष DriveType::MULTIPLE:
		h.eat(multiple_);
		रोक;
	}
	h.eat(type_);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveChunkWire::hash_into(Hasher h) अचर
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveChunkPort::hash_into(Hasher h) अचर
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveChunkMarker::hash_into(Hasher h) अचर
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveChunkMultiple::hash_into(Hasher h) अचर
{
	h.eat(width_);
	h.eat(multiple_);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveChunk::hash_into(Hasher h) अचर
{
	विकल्प (type_) {
	पक्ष DriveType::NONE:
		h.eat(0);
		रोक;
	पक्ष DriveType::CONSTANT:
		h.eat(constant_);
		रोक;
	पक्ष DriveType::WIRE:
		h.eat(wire_);
		रोक;
	पक्ष DriveType::PORT:
		h.eat(port_);
		रोक;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		रोक;
	पक्ष DriveType::MULTIPLE:
		h.eat(multiple_);
		रोक;
	}
	h.eat(type_);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriveSpec::hash_into(Hasher h) अचर
{
	updhash();
	h.eat(hash_);
	फर्काऊ h;
}

अन्तःस्थ Hasher DriverMap::DriveBitId::hash_into(Hasher h) अचर
{
	h.eat(id);
	फर्काऊ h;
}

YOSYS_NAMESPACE_END

#endif
