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

std::string log_signal(DriveChunkWire ध्रुव &chunk);
std::string log_signal(DriveChunkPort ध्रुव &chunk);
std::string log_signal(DriveChunkMarker ध्रुव &chunk);
std::string log_signal(DriveChunk ध्रुव &chunk);
std::string log_signal(DriveSpec ध्रुव &chunk);

गणना class DriveType : unsigned वर्ण
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

	bool operator==(ध्रुव DriveBitWire &other) ध्रुव
	{
		प्रत्यागम wire == other.wire && offset == other.offset;
	}

	bool operator<(ध्रुव DriveBitWire &other) ध्रुव
	{
		यदि (wire != other.wire)
			प्रत्यागम wire->name < other.wire->name;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;


	operator SigBit() ध्रुव
	{
		प्रत्यागम SigBit(wire, offset);
	}
};

संरचना DriveBitPort
{
	Cell *cell;
	IdString port;
	पूर्णाङ्क offset;

	DriveBitPort(Cell *cell, IdString port, पूर्णाङ्क offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(ध्रुव DriveBitPort &other) ध्रुव
	{
		प्रत्यागम cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(ध्रुव DriveBitPort &other) ध्रुव
	{
		यदि (cell != other.cell)
			प्रत्यागम cell->name < other.cell->name;
		यदि (port != other.port)
			प्रत्यागम port < other.port;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

};


संरचना DriveBitMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क offset;

	DriveBitMarker(पूर्णाङ्क marker, पूर्णाङ्क offset) : marker(marker), offset(offset) {}

	bool operator==(ध्रुव DriveBitMarker &other) ध्रुव
	{
		प्रत्यागम marker == other.marker && offset == other.offset;
	}

	bool operator<(ध्रुव DriveBitMarker &other) ध्रुव
	{
		यदि (marker != other.marker)
			प्रत्यागम marker < other.marker;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

};

संरचना DriveBitMultiple
{
गुप्त:
	pool<DriveBit> multiple_;

सार्वजनिक:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit ध्रुव &single);

	pool<DriveBit> ध्रुव &multiple() ध्रुव { प्रत्यागम multiple_; }

	शून्य merge(DriveBitMultiple ध्रुव &other)
	{
		प्रति (DriveBit ध्रुव &single : other.multiple_)
			merge(single);
	}

	शून्य merge(DriveBitMultiple &&other)
	{
		प्रति (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	शून्य merge(DriveBit ध्रुव &single);
	शून्य merge(DriveBit &&single);

	bool operator==(ध्रुव DriveBitMultiple &other) ध्रुव
	{
		प्रत्यागम multiple_ == other.multiple_;
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
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
सार्वजनिक:
	DriveBit()  {}

	DriveBit(SigBit ध्रुव &bit);

	DriveBit(DriveBit ध्रुव &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire ध्रुव &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort ध्रुव &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker ध्रुव &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple ध्रुव &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	शून्य set_none()
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
			पक्ष DriveType::PORT:
				port_.~DriveBitPort();
				भङ्ग;
			पक्ष DriveType::MARKER:
				marker_.~DriveBitMarker();
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				भङ्ग;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit ध्रुव &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none();
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				भङ्ग;
			पक्ष DriveType::PORT:
				*this = other.port_;
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				*this = other.multiple_;
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none();
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				भङ्ग;
			पक्ष DriveType::PORT:
				*this = std::move(other.port_);
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitWire ध्रुव &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitPort ध्रुव &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitMarker ध्रुव &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitMultiple ध्रुव &multiple)
	{
		set_none();
		यदि (multiple.multiple().empty())
			प्रत्यागम *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		प्रत्यागम *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		यदि (multiple.multiple().empty())
			प्रत्यागम *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		प्रत्यागम *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool operator==(ध्रुव DriveBit &other) ध्रुव
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
			पक्ष DriveType::PORT:
				प्रत्यागम port_ == other.port_;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ == other.marker_;
			पक्ष DriveType::MULTIPLE:
				प्रत्यागम multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ध्रुव DriveBit &other) ध्रुव
	{
		प्रत्यागम !(*this == other);
	}

	bool operator<(ध्रुव DriveBit &other) ध्रुव
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
			पक्ष DriveType::PORT:
				प्रत्यागम port_ < other.port_;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ < other.marker_;
			पक्ष DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ध्रुव { प्रत्यागम type_; }

	bool is_none() ध्रुव { प्रत्यागम type_ == DriveType::NONE; }
	bool is_constant() ध्रुव { प्रत्यागम type_ == DriveType::CONSTANT; }
	bool is_wire() ध्रुव { प्रत्यागम type_ == DriveType::WIRE; }
	bool is_port() ध्रुव { प्रत्यागम type_ == DriveType::PORT; }
	bool is_marker() ध्रुव { प्रत्यागम type_ == DriveType::MARKER; }
	bool is_multiple() ध्रुव { प्रत्यागम type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); प्रत्यागम constant_; }
	State ध्रुव &constant() ध्रुव { log_assert(is_constant()); प्रत्यागम constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveBitWire ध्रुव &wire() ध्रुव { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveBitPort &port() { log_assert(is_port()); प्रत्यागम port_; }
	DriveBitPort ध्रुव &port() ध्रुव { log_assert(is_port()); प्रत्यागम port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveBitMarker ध्रुव &marker() ध्रुव { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); प्रत्यागम multiple_; }
	DriveBitMultiple ध्रुव &multiple() ध्रुव { log_assert(is_multiple()); प्रत्यागम multiple_; }

	शून्य merge(DriveBit ध्रुव &other);

};

अन्तःस्थ DriveBitMultiple::DriveBitMultiple() {}
अन्तःस्थ DriveBitMultiple::DriveBitMultiple(DriveBit ध्रुव &single)
{
	multiple_.emplace(single);
}

संरचना DriveChunkWire
{
	Wire *wire;
	पूर्णाङ्क offset;
	पूर्णाङ्क width;

	DriveChunkWire(Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire ध्रुव &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	पूर्णाङ्क size() ध्रुव { प्रत्यागम width; }

	DriveBitWire operator[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < width);
		प्रत्यागम DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire ध्रुव &bit) ध्रुव;
	bool try_append(DriveBitWire ध्रुव &bit);
	bool try_append(DriveChunkWire ध्रुव &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() ध्रुव { प्रत्यागम offset == 0 && width == wire->width; }

	bool operator==(ध्रुव DriveChunkWire &other) ध्रुव
	{
		प्रत्यागम wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(ध्रुव DriveChunkWire &other) ध्रुव
	{
		यदि (wire != other.wire)
			प्रत्यागम wire->name < other.wire->name;
		यदि (width != other.width)
			प्रत्यागम width < other.width;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	explicit operator SigChunk() ध्रुव
	{
		प्रत्यागम SigChunk(wire, offset, width);
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
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> ध्रुव &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort ध्रुव &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	पूर्णाङ्क size() ध्रुव { प्रत्यागम width; }

	DriveBitPort operator[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < width);
		प्रत्यागम DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort ध्रुव &bit) ध्रुव;
	bool try_append(DriveBitPort ध्रुव &bit);
	bool try_append(DriveChunkPort ध्रुव &chunk);

	// Whether this chunk is a whole port
	bool is_whole() ध्रुव
	{
		प्रत्यागम offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(ध्रुव DriveChunkPort &other) ध्रुव
	{
		प्रत्यागम cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(ध्रुव DriveChunkPort &other) ध्रुव
	{
		यदि (cell != other.cell)
			प्रत्यागम cell->name < other.cell->name;
		यदि (port != other.port)
			प्रत्यागम port < other.port;
		यदि (width != other.width)
			प्रत्यागम width < other.width;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};


संरचना DriveChunkMarker
{
	पूर्णाङ्क marker;
	पूर्णाङ्क offset;
	पूर्णाङ्क width;

	DriveChunkMarker(पूर्णाङ्क marker, पूर्णाङ्क offset, पूर्णाङ्क width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker ध्रुव &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	पूर्णाङ्क size() ध्रुव { प्रत्यागम width; }

	DriveBitMarker operator[](पूर्णाङ्क i) ध्रुव
	{
		log_assert(i >= 0 && i < width);
		प्रत्यागम DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker ध्रुव &bit) ध्रुव;
	bool try_append(DriveBitMarker ध्रुव &bit);
	bool try_append(DriveChunkMarker ध्रुव &chunk);

	bool operator==(ध्रुव DriveChunkMarker &other) ध्रुव
	{
		प्रत्यागम marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(ध्रुव DriveChunkMarker &other) ध्रुव
	{
		यदि (marker != other.marker)
			प्रत्यागम marker < other.marker;
		यदि (width != other.width)
			प्रत्यागम width < other.width;
		प्रत्यागम offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना DriveChunkMultiple
{
गुप्त:
	mutable pool<DriveChunk> multiple_;
	पूर्णाङ्क width_;

सार्वजनिक:
	pool<DriveChunk> ध्रुव &multiple() ध्रुव { प्रत्यागम multiple_; }

	DriveChunkMultiple(DriveBitMultiple ध्रुव &bit);

	पूर्णाङ्क size() ध्रुव { प्रत्यागम width_; }

	DriveBitMultiple operator[](पूर्णाङ्क i) ध्रुव;

	bool can_append(DriveBitMultiple ध्रुव &bit) ध्रुव;

	bool try_append(DriveBitMultiple ध्रुव &bit);


	bool can_append(DriveChunkMultiple ध्रुव &bit) ध्रुव;

	bool try_append(DriveChunkMultiple ध्रुव &bit);

	bool operator==(ध्रुव DriveChunkMultiple &other) ध्रुव
	{
		प्रत्यागम width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(ध्रुव DriveChunkMultiple &other) ध्रुव
	{
		यदि (multiple_.size() < other.multiple_.size())

		multiple_.sort();
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
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

सार्वजनिक:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk ध्रुव &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit ध्रुव &other) { *this = other; }

	DriveChunk(Const ध्रुव &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ध्रुव &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort ध्रुव &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker ध्रुव &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple ध्रुव &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](पूर्णाङ्क i) ध्रुव
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम DriveBit();
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_[i];
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_[i];
			पक्ष DriveType::PORT:
				प्रत्यागम port_[i];
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_[i];
			पक्ष DriveType::MULTIPLE:
				प्रत्यागम multiple_[i];
		}
		log_abort();
	}

	शून्य set_none(पूर्णाङ्क width = 0)
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				none_ = width;
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				constant_.~Const();
				भङ्ग;
			पक्ष DriveType::WIRE:
				wire_.~DriveChunkWire();
				भङ्ग;
			पक्ष DriveType::PORT:
				port_.~DriveChunkPort();
				भङ्ग;
			पक्ष DriveType::MARKER:
				marker_.~DriveChunkMarker();
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				भङ्ग;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk ध्रुव &other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none(other.none_);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = other.constant_;
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = other.wire_;
				भङ्ग;
			पक्ष DriveType::PORT:
				*this = other.port_;
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = other.marker_;
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				*this = other.multiple_;
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		विकल्प (other.type_)
		{
			पक्ष DriveType::NONE:
				set_none(other.none_);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = std::move(other.constant_);
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = std::move(other.wire_);
				भङ्ग;
			पक्ष DriveType::PORT:
				*this = std::move(other.port_);
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = std::move(other.marker_);
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	DriveChunk &operator=(Const ध्रुव &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkWire ध्रुव &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkPort ध्रुव &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkMarker ध्रुव &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkMultiple ध्रुव &multiple)
	{
		set_none(multiple.size());
		यदि (multiple.multiple().empty())
			प्रत्यागम *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		यदि (multiple.multiple().empty())
			प्रत्यागम *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		प्रत्यागम *this;
	}

	DriveChunk &operator=(DriveBit ध्रुव &other)
	{
		विकल्प (other.type())
		{
			पक्ष DriveType::NONE:
				set_none(1);
				भङ्ग;
			पक्ष DriveType::CONSTANT:
				*this = Const(other.constant());
				भङ्ग;
			पक्ष DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				भङ्ग;
			पक्ष DriveType::PORT:
				*this = DriveChunkPort(other.port());
				भङ्ग;
			पक्ष DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				भङ्ग;
			पक्ष DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				भङ्ग;
		}
		प्रत्यागम *this;
	}

	bool can_append(DriveBit ध्रुव &bit) ध्रुव;
	bool try_append(DriveBit ध्रुव &bit);
	bool try_append(DriveChunk ध्रुव &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool operator==(ध्रुव DriveChunk &other) ध्रुव
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
			पक्ष DriveType::PORT:
				प्रत्यागम port_ == other.port_;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ == other.marker_;
			पक्ष DriveType::MULTIPLE:
				प्रत्यागम multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(ध्रुव DriveChunk &other) ध्रुव
	{
		प्रत्यागम !(*this == other);
	}

	bool operator<(ध्रुव DriveChunk &other) ध्रुव
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
			पक्ष DriveType::PORT:
				प्रत्यागम port_ < other.port_;
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_ < other.marker_;
			पक्ष DriveType::MULTIPLE:
				प्रत्यागम multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() ध्रुव { प्रत्यागम type_; }

	bool is_none() ध्रुव { प्रत्यागम type_ == DriveType::NONE; }
	bool is_constant() ध्रुव { प्रत्यागम type_ == DriveType::CONSTANT; }
	bool is_wire() ध्रुव { प्रत्यागम type_ == DriveType::WIRE; }
	bool is_port() ध्रुव { प्रत्यागम type_ == DriveType::PORT; }
	bool is_marker() ध्रुव { प्रत्यागम type_ == DriveType::MARKER; }
	bool is_multiple() ध्रुव { प्रत्यागम type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); प्रत्यागम constant_; }
	Const ध्रुव &constant() ध्रुव { log_assert(is_constant()); प्रत्यागम constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveChunkWire ध्रुव &wire() ध्रुव { log_assert(is_wire()); प्रत्यागम wire_; }
	DriveChunkPort &port() { log_assert(is_port()); प्रत्यागम port_; }
	DriveChunkPort ध्रुव &port() ध्रुव { log_assert(is_port()); प्रत्यागम port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveChunkMarker ध्रुव &marker() ध्रुव { log_assert(is_marker()); प्रत्यागम marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); प्रत्यागम multiple_; }
	DriveChunkMultiple ध्रुव &multiple() ध्रुव { log_assert(is_multiple()); प्रत्यागम multiple_; }


	पूर्णाङ्क size() ध्रुव
	{
		विकल्प (type_)
		{
			पक्ष DriveType::NONE:
				प्रत्यागम none_;
			पक्ष DriveType::CONSTANT:
				प्रत्यागम constant_.size();
			पक्ष DriveType::WIRE:
				प्रत्यागम wire_.size();
			पक्ष DriveType::PORT:
				प्रत्यागम port_.size();
			पक्ष DriveType::MARKER:
				प्रत्यागम marker_.size();
			पक्ष DriveType::MULTIPLE:
				प्रत्यागम multiple_.size();
		}
		log_abort();
	}
};

अन्तःस्थ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple ध्रुव &bit)
	: width_(1)
{
	प्रति (auto ध्रुव &bit : bit.multiple())
		multiple_.emplace(bit);
}

संरचना DriveSpec
{
गुप्त:
	पूर्णाङ्क width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned पूर्णाङ्क hash_ = 0;
सार्वजनिक:

	अन्तःस्थ bool packed() ध्रुव {
		प्रत्यागम bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ध्रुव &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple ध्रुव &chunk) { *this = chunk; }

	DriveSpec(DriveBit ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitWire ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitPort ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitMarker ध्रुव &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple ध्रुव &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> ध्रुव &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> ध्रुव &bits)
	{
		प्रति (auto ध्रुव &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec ध्रुव &sig)
	{
		// TODO: converting one chunk at a time would be faster
		प्रति (auto ध्रुव &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> ध्रुव &chunks() ध्रुव { pack(); प्रत्यागम chunks_; }
	std::vector<DriveBit> ध्रुव &bits() ध्रुव { unpack(); प्रत्यागम bits_; }

	पूर्णाङ्क size() ध्रुव { प्रत्यागम width_; }

	शून्य append(DriveBit ध्रुव &bit);

	शून्य append(DriveChunk ध्रुव &chunk);

	शून्य pack() ध्रुव;

	शून्य unpack() ध्रुव;

	DriveBit &operator[](पूर्णाङ्क index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		प्रत्यागम bits_[index];
	}

	ध्रुव DriveBit &operator[](पूर्णाङ्क index) ध्रुव
	{
		log_assert(index >= 0 && index < size());
		unpack();
		प्रत्यागम bits_[index];
	}

	शून्य clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk ध्रुव &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		प्रत्यागम *this;
	}

	DriveSpec &operator=(DriveChunkWire ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple ध्रुव &chunk) { प्रत्यागम *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit ध्रुव &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		प्रत्यागम *this;
	}

	DriveSpec &operator=(DriveBitWire ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple ध्रुव &bit) { प्रत्यागम *this = DriveBit(bit); }

	शून्य updhash() ध्रुव {
		यदि (hash_ != 0)
			प्रत्यागम;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;

	bool operator==(DriveSpec ध्रुव &other) ध्रुव {
		updhash();
		other.updhash();
		यदि (size() != other.size() || hash_ != other.hash_)
			प्रत्यागम false;
		प्रत्यागम chunks() == other.chunks();
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

		bool operator==(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id == other.id; }
		bool operator!=(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id != other.id; }
		bool operator<(ध्रुव DriveBitId &other) ध्रुव { प्रत्यागम id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	संरचना DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		शून्य add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId निकालो_edge(DriveBitId src);
		शून्य clear(DriveBitId src);
		bool contains(DriveBitId src);
		पूर्णाङ्क गिनती(DriveBitId src);

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

	BitMode bit_mode(DriveBit ध्रुव &bit);
	DriveBitId id_from_drive_bit(DriveBit ध्रुव &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	शून्य connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_undirected(DriveBitId a_id, DriveBitId b_id);

सार्वजनिक:

	शून्य add(Module *module);

	// Add a single bit connection to the driver map.
	शून्य add(DriveBit ध्रुव &a, DriveBit ध्रुव &b);

	template<typename T>
	स्थिर constexpr bool is_sig_type() {
		प्रत्यागम
			std::is_same<T, SigSpec>::मूल्य ||
			std::is_same<T, SigChunk>::मूल्य ||
			std::is_same<T, DriveSpec>::मूल्य ||
			std::is_same<T, DriveChunk>::मूल्य ||
			std::is_same<T, DriveChunkPort>::मूल्य ||
			std::is_same<T, DriveChunkWire>::मूल्य ||
			std::is_same<T, Const>::मूल्य;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T ध्रुव &a, U ध्रुव &b)
	{
		log_assert(a.size() == b.size());
		प्रति (पूर्णाङ्क i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	शून्य add(SigSpec ध्रुव &a, SigSpec ध्रुव &b);

गुप्त:
	शून्य add_port(Cell *cell, IdString port, SigSpec ध्रुव &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, पूर्णाङ्क> orient_undirected_distance;

	शून्य orient_undirected(DriveBitId id);

सार्वजनिक:
	DriveBit operator()(DriveBit ध्रुव &bit);

	DriveSpec operator()(DriveSpec spec);

गुप्त:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		प्रत्यागम wire->has_attribute(ID(keep));
	}
};

अन्तःस्थ Hasher DriveBitWire::hash_into(Hasher h) ध्रुव
{
	h.eat(wire->name);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveBitPort::hash_into(Hasher h) ध्रुव
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveBitMarker::hash_into(Hasher h) ध्रुव
{
	h.eat(marker);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveBitMultiple::hash_into(Hasher h) ध्रुव
{
	h.eat(multiple_);
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
	पक्ष DriveType::PORT:
		h.eat(port_);
		भङ्ग;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		भङ्ग;
	पक्ष DriveType::MULTIPLE:
		h.eat(multiple_);
		भङ्ग;
	}
	h.eat(type_);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkWire::hash_into(Hasher h) ध्रुव
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkPort::hash_into(Hasher h) ध्रुव
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkMarker::hash_into(Hasher h) ध्रुव
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	प्रत्यागम h;
}

अन्तःस्थ Hasher DriveChunkMultiple::hash_into(Hasher h) ध्रुव
{
	h.eat(width_);
	h.eat(multiple_);
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
	पक्ष DriveType::PORT:
		h.eat(port_);
		भङ्ग;
	पक्ष DriveType::MARKER:
		h.eat(marker_);
		भङ्ग;
	पक्ष DriveType::MULTIPLE:
		h.eat(multiple_);
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

YOSYS_NAMESPACE_END

#endif
