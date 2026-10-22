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

std::string log_signal(DriveChunkWire स्थिरांक &chunk);
std::string log_signal(DriveChunkPort स्थिरांक &chunk);
std::string log_signal(DriveChunkMarker स्थिरांक &chunk);
std::string log_signal(DriveChunk स्थिरांक &chunk);
std::string log_signal(DriveSpec स्थिरांक &chunk);

गणना class DriveType : अचिन्हित अक्षर
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
	पूर्णांक offset;

	DriveBitWire(Wire *wire, पूर्णांक offset) : wire(wire), offset(offset) {}

	bool चालक==(स्थिरांक DriveBitWire &other) स्थिरांक
	{
		परत wire == other.wire && offset == other.offset;
	}

	bool चालक<(स्थिरांक DriveBitWire &other) स्थिरांक
	{
		जर (wire != other.wire)
			परत wire->name < other.wire->name;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;


	चालक SigBit() स्थिरांक
	{
		परत SigBit(wire, offset);
	}
};

संरचना DriveBitPort
{
	Cell *cell;
	IdString port;
	पूर्णांक offset;

	DriveBitPort(Cell *cell, IdString port, पूर्णांक offset) : cell(cell), port(port), offset(offset) {}

	bool चालक==(स्थिरांक DriveBitPort &other) स्थिरांक
	{
		परत cell == other.cell && port == other.port && offset == other.offset;
	}

	bool चालक<(स्थिरांक DriveBitPort &other) स्थिरांक
	{
		जर (cell != other.cell)
			परत cell->name < other.cell->name;
		जर (port != other.port)
			परत port < other.port;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

};


संरचना DriveBitMarker
{
	पूर्णांक marker;
	पूर्णांक offset;

	DriveBitMarker(पूर्णांक marker, पूर्णांक offset) : marker(marker), offset(offset) {}

	bool चालक==(स्थिरांक DriveBitMarker &other) स्थिरांक
	{
		परत marker == other.marker && offset == other.offset;
	}

	bool चालक<(स्थिरांक DriveBitMarker &other) स्थिरांक
	{
		जर (marker != other.marker)
			परत marker < other.marker;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

};

संरचना DriveBitMultiple
{
खाजगी:
	pool<DriveBit> multiple_;

सार्वजनिक:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit स्थिरांक &single);

	pool<DriveBit> स्थिरांक &multiple() स्थिरांक { परत multiple_; }

	शून्य merge(DriveBitMultiple स्थिरांक &other)
	{
		साठी (DriveBit स्थिरांक &single : other.multiple_)
			merge(single);
	}

	शून्य merge(DriveBitMultiple &&other)
	{
		साठी (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	शून्य merge(DriveBit स्थिरांक &single);
	शून्य merge(DriveBit &&single);

	bool चालक==(स्थिरांक DriveBitMultiple &other) स्थिरांक
	{
		परत multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
};

संरचना DriveBit
{
खाजगी:
	DriveType type_ = DriveType::NONE;
	संघ
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
सार्वजनिक:
	DriveBit()  {}

	DriveBit(SigBit स्थिरांक &bit);

	DriveBit(DriveBit स्थिरांक &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire स्थिरांक &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort स्थिरांक &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker स्थिरांक &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple स्थिरांक &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	शून्य set_none()
	{
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				थांबा;
			स्थिती DriveType::CONSTANT:
				थांबा;
			स्थिती DriveType::WIRE:
				wire_.~DriveBitWire();
				थांबा;
			स्थिती DriveType::PORT:
				port_.~DriveBitPort();
				थांबा;
			स्थिती DriveType::MARKER:
				marker_.~DriveBitMarker();
				थांबा;
			स्थिती DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				थांबा;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &चालक=(DriveBit स्थिरांक &other)
	{
		निवड (other.type_)
		{
			स्थिती DriveType::NONE:
				set_none();
				थांबा;
			स्थिती DriveType::CONSTANT:
				*this = other.constant_;
				थांबा;
			स्थिती DriveType::WIRE:
				*this = other.wire_;
				थांबा;
			स्थिती DriveType::PORT:
				*this = other.port_;
				थांबा;
			स्थिती DriveType::MARKER:
				*this = other.marker_;
				थांबा;
			स्थिती DriveType::MULTIPLE:
				*this = other.multiple_;
				थांबा;
		}
		परत *this;
	}

	DriveBit &चालक=(DriveBit &&other)
	{
		निवड (other.type_)
		{
			स्थिती DriveType::NONE:
				set_none();
				थांबा;
			स्थिती DriveType::CONSTANT:
				*this = std::move(other.constant_);
				थांबा;
			स्थिती DriveType::WIRE:
				*this = std::move(other.wire_);
				थांबा;
			स्थिती DriveType::PORT:
				*this = std::move(other.port_);
				थांबा;
			स्थिती DriveType::MARKER:
				*this = std::move(other.marker_);
				थांबा;
			स्थिती DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				थांबा;
		}
		परत *this;
	}

	DriveBit &चालक=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		परत *this;
	}

	DriveBit &चालक=(DriveBitWire स्थिरांक &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		परत *this;
	}

	DriveBit &चालक=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		परत *this;
	}

	DriveBit &चालक=(DriveBitPort स्थिरांक &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		परत *this;
	}

	DriveBit &चालक=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		परत *this;
	}

	DriveBit &चालक=(DriveBitMarker स्थिरांक &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		परत *this;
	}

	DriveBit &चालक=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		परत *this;
	}

	DriveBit &चालक=(DriveBitMultiple स्थिरांक &multiple)
	{
		set_none();
		जर (multiple.multiple().empty())
			परत *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		परत *this;
	}

	DriveBit &चालक=(DriveBitMultiple &&multiple)
	{
		set_none();
		जर (multiple.multiple().empty())
			परत *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		परत *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

	bool चालक==(स्थिरांक DriveBit &other) स्थिरांक
	{
		जर (type_ != other.type_)
			परत false;

		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत true;
			स्थिती DriveType::CONSTANT:
				परत constant_ == other.constant_;
			स्थिती DriveType::WIRE:
				परत wire_ == other.wire_;
			स्थिती DriveType::PORT:
				परत port_ == other.port_;
			स्थिती DriveType::MARKER:
				परत marker_ == other.marker_;
			स्थिती DriveType::MULTIPLE:
				परत multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(स्थिरांक DriveBit &other) स्थिरांक
	{
		परत !(*this == other);
	}

	bool चालक<(स्थिरांक DriveBit &other) स्थिरांक
	{
		जर (type_ != other.type_)
			परत type_ < other.type_;
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत false;
			स्थिती DriveType::CONSTANT:
				परत constant_ < other.constant_;
			स्थिती DriveType::WIRE:
				परत wire_ < other.wire_;
			स्थिती DriveType::PORT:
				परत port_ < other.port_;
			स्थिती DriveType::MARKER:
				परत marker_ < other.marker_;
			स्थिती DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() स्थिरांक { परत type_; }

	bool is_none() स्थिरांक { परत type_ == DriveType::NONE; }
	bool is_constant() स्थिरांक { परत type_ == DriveType::CONSTANT; }
	bool is_wire() स्थिरांक { परत type_ == DriveType::WIRE; }
	bool is_port() स्थिरांक { परत type_ == DriveType::PORT; }
	bool is_marker() स्थिरांक { परत type_ == DriveType::MARKER; }
	bool is_multiple() स्थिरांक { परत type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); परत constant_; }
	State स्थिरांक &constant() स्थिरांक { log_assert(is_constant()); परत constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); परत wire_; }
	DriveBitWire स्थिरांक &wire() स्थिरांक { log_assert(is_wire()); परत wire_; }
	DriveBitPort &port() { log_assert(is_port()); परत port_; }
	DriveBitPort स्थिरांक &port() स्थिरांक { log_assert(is_port()); परत port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); परत marker_; }
	DriveBitMarker स्थिरांक &marker() स्थिरांक { log_assert(is_marker()); परत marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); परत multiple_; }
	DriveBitMultiple स्थिरांक &multiple() स्थिरांक { log_assert(is_multiple()); परत multiple_; }

	शून्य merge(DriveBit स्थिरांक &other);

};

ओळीत DriveBitMultiple::DriveBitMultiple() {}
ओळीत DriveBitMultiple::DriveBitMultiple(DriveBit स्थिरांक &single)
{
	multiple_.emplace(single);
}

संरचना DriveChunkWire
{
	Wire *wire;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkWire(Wire *wire, पूर्णांक offset, पूर्णांक width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire स्थिरांक &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	पूर्णांक size() स्थिरांक { परत width; }

	DriveBitWire चालक[](पूर्णांक i) स्थिरांक
	{
		log_assert(i >= 0 && i < width);
		परत DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire स्थिरांक &bit) स्थिरांक;
	bool try_append(DriveBitWire स्थिरांक &bit);
	bool try_append(DriveChunkWire स्थिरांक &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() स्थिरांक { परत offset == 0 && width == wire->width; }

	bool चालक==(स्थिरांक DriveChunkWire &other) स्थिरांक
	{
		परत wire == other.wire && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिरांक DriveChunkWire &other) स्थिरांक
	{
		जर (wire != other.wire)
			परत wire->name < other.wire->name;
		जर (width != other.width)
			परत width < other.width;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

	explicit चालक SigChunk() स्थिरांक
	{
		परत SigChunk(wire, offset, width);
	}
};

संरचना DriveChunkPort
{
	Cell *cell;
	IdString port;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkPort(Cell *cell, IdString port, पूर्णांक offset, पूर्णांक width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> स्थिरांक &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort स्थिरांक &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	पूर्णांक size() स्थिरांक { परत width; }

	DriveBitPort चालक[](पूर्णांक i) स्थिरांक
	{
		log_assert(i >= 0 && i < width);
		परत DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort स्थिरांक &bit) स्थिरांक;
	bool try_append(DriveBitPort स्थिरांक &bit);
	bool try_append(DriveChunkPort स्थिरांक &chunk);

	// Whether this chunk is a whole port
	bool is_whole() स्थिरांक
	{
		परत offset == 0 && width == cell->connections().at(port).size();
	}

	bool चालक==(स्थिरांक DriveChunkPort &other) स्थिरांक
	{
		परत cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिरांक DriveChunkPort &other) स्थिरांक
	{
		जर (cell != other.cell)
			परत cell->name < other.cell->name;
		जर (port != other.port)
			परत port < other.port;
		जर (width != other.width)
			परत width < other.width;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
};


संरचना DriveChunkMarker
{
	पूर्णांक marker;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkMarker(पूर्णांक marker, पूर्णांक offset, पूर्णांक width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker स्थिरांक &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	पूर्णांक size() स्थिरांक { परत width; }

	DriveBitMarker चालक[](पूर्णांक i) स्थिरांक
	{
		log_assert(i >= 0 && i < width);
		परत DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker स्थिरांक &bit) स्थिरांक;
	bool try_append(DriveBitMarker स्थिरांक &bit);
	bool try_append(DriveChunkMarker स्थिरांक &chunk);

	bool चालक==(स्थिरांक DriveChunkMarker &other) स्थिरांक
	{
		परत marker == other.marker && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिरांक DriveChunkMarker &other) स्थिरांक
	{
		जर (marker != other.marker)
			परत marker < other.marker;
		जर (width != other.width)
			परत width < other.width;
		परत offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
};

संरचना DriveChunkMultiple
{
खाजगी:
	mutable pool<DriveChunk> multiple_;
	पूर्णांक width_;

सार्वजनिक:
	pool<DriveChunk> स्थिरांक &multiple() स्थिरांक { परत multiple_; }

	DriveChunkMultiple(DriveBitMultiple स्थिरांक &bit);

	पूर्णांक size() स्थिरांक { परत width_; }

	DriveBitMultiple चालक[](पूर्णांक i) स्थिरांक;

	bool can_append(DriveBitMultiple स्थिरांक &bit) स्थिरांक;

	bool try_append(DriveBitMultiple स्थिरांक &bit);


	bool can_append(DriveChunkMultiple स्थिरांक &bit) स्थिरांक;

	bool try_append(DriveChunkMultiple स्थिरांक &bit);

	bool चालक==(स्थिरांक DriveChunkMultiple &other) स्थिरांक
	{
		परत width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool चालक<(स्थिरांक DriveChunkMultiple &other) स्थिरांक
	{
		जर (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		परत false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
};

संरचना DriveChunk
{
खाजगी:
	DriveType type_ = DriveType::NONE;
	संघ
	{
		पूर्णांक none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

सार्वजनिक:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk स्थिरांक &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit स्थिरांक &other) { *this = other; }

	DriveChunk(Const स्थिरांक &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire स्थिरांक &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort स्थिरांक &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker स्थिरांक &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple स्थिरांक &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit चालक[](पूर्णांक i) स्थिरांक
	{
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत DriveBit();
			स्थिती DriveType::CONSTANT:
				परत constant_[i];
			स्थिती DriveType::WIRE:
				परत wire_[i];
			स्थिती DriveType::PORT:
				परत port_[i];
			स्थिती DriveType::MARKER:
				परत marker_[i];
			स्थिती DriveType::MULTIPLE:
				परत multiple_[i];
		}
		log_abort();
	}

	शून्य set_none(पूर्णांक width = 0)
	{
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				none_ = width;
				थांबा;
			स्थिती DriveType::CONSTANT:
				constant_.~Const();
				थांबा;
			स्थिती DriveType::WIRE:
				wire_.~DriveChunkWire();
				थांबा;
			स्थिती DriveType::PORT:
				port_.~DriveChunkPort();
				थांबा;
			स्थिती DriveType::MARKER:
				marker_.~DriveChunkMarker();
				थांबा;
			स्थिती DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				थांबा;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &चालक=(DriveChunk स्थिरांक &other)
	{
		निवड (other.type_)
		{
			स्थिती DriveType::NONE:
				set_none(other.none_);
				थांबा;
			स्थिती DriveType::CONSTANT:
				*this = other.constant_;
				थांबा;
			स्थिती DriveType::WIRE:
				*this = other.wire_;
				थांबा;
			स्थिती DriveType::PORT:
				*this = other.port_;
				थांबा;
			स्थिती DriveType::MARKER:
				*this = other.marker_;
				थांबा;
			स्थिती DriveType::MULTIPLE:
				*this = other.multiple_;
				थांबा;
		}
		परत *this;
	}

	DriveChunk &चालक=(DriveChunk &&other)
	{
		निवड (other.type_)
		{
			स्थिती DriveType::NONE:
				set_none(other.none_);
				थांबा;
			स्थिती DriveType::CONSTANT:
				*this = std::move(other.constant_);
				थांबा;
			स्थिती DriveType::WIRE:
				*this = std::move(other.wire_);
				थांबा;
			स्थिती DriveType::PORT:
				*this = std::move(other.port_);
				थांबा;
			स्थिती DriveType::MARKER:
				*this = std::move(other.marker_);
				थांबा;
			स्थिती DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				थांबा;
		}
		परत *this;
	}

	DriveChunk &चालक=(Const स्थिरांक &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		परत *this;
	}

	DriveChunk &चालक=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkWire स्थिरांक &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkPort स्थिरांक &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkMarker स्थिरांक &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple स्थिरांक &multiple)
	{
		set_none(multiple.size());
		जर (multiple.multiple().empty())
			परत *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		परत *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		जर (multiple.multiple().empty())
			परत *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		परत *this;
	}

	DriveChunk &चालक=(DriveBit स्थिरांक &other)
	{
		निवड (other.type())
		{
			स्थिती DriveType::NONE:
				set_none(1);
				थांबा;
			स्थिती DriveType::CONSTANT:
				*this = Const(other.constant());
				थांबा;
			स्थिती DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				थांबा;
			स्थिती DriveType::PORT:
				*this = DriveChunkPort(other.port());
				थांबा;
			स्थिती DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				थांबा;
			स्थिती DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				थांबा;
		}
		परत *this;
	}

	bool can_append(DriveBit स्थिरांक &bit) स्थिरांक;
	bool try_append(DriveBit स्थिरांक &bit);
	bool try_append(DriveChunk स्थिरांक &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

	bool चालक==(स्थिरांक DriveChunk &other) स्थिरांक
	{
		जर (type_ != other.type_)
			परत false;

		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत true;
			स्थिती DriveType::CONSTANT:
				परत constant_ == other.constant_;
			स्थिती DriveType::WIRE:
				परत wire_ == other.wire_;
			स्थिती DriveType::PORT:
				परत port_ == other.port_;
			स्थिती DriveType::MARKER:
				परत marker_ == other.marker_;
			स्थिती DriveType::MULTIPLE:
				परत multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(स्थिरांक DriveChunk &other) स्थिरांक
	{
		परत !(*this == other);
	}

	bool चालक<(स्थिरांक DriveChunk &other) स्थिरांक
	{
		जर (type_ != other.type_)
			परत type_ < other.type_;
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत false;
			स्थिती DriveType::CONSTANT:
				परत constant_ < other.constant_;
			स्थिती DriveType::WIRE:
				परत wire_ < other.wire_;
			स्थिती DriveType::PORT:
				परत port_ < other.port_;
			स्थिती DriveType::MARKER:
				परत marker_ < other.marker_;
			स्थिती DriveType::MULTIPLE:
				परत multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() स्थिरांक { परत type_; }

	bool is_none() स्थिरांक { परत type_ == DriveType::NONE; }
	bool is_constant() स्थिरांक { परत type_ == DriveType::CONSTANT; }
	bool is_wire() स्थिरांक { परत type_ == DriveType::WIRE; }
	bool is_port() स्थिरांक { परत type_ == DriveType::PORT; }
	bool is_marker() स्थिरांक { परत type_ == DriveType::MARKER; }
	bool is_multiple() स्थिरांक { परत type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); परत constant_; }
	Const स्थिरांक &constant() स्थिरांक { log_assert(is_constant()); परत constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); परत wire_; }
	DriveChunkWire स्थिरांक &wire() स्थिरांक { log_assert(is_wire()); परत wire_; }
	DriveChunkPort &port() { log_assert(is_port()); परत port_; }
	DriveChunkPort स्थिरांक &port() स्थिरांक { log_assert(is_port()); परत port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); परत marker_; }
	DriveChunkMarker स्थिरांक &marker() स्थिरांक { log_assert(is_marker()); परत marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); परत multiple_; }
	DriveChunkMultiple स्थिरांक &multiple() स्थिरांक { log_assert(is_multiple()); परत multiple_; }


	पूर्णांक size() स्थिरांक
	{
		निवड (type_)
		{
			स्थिती DriveType::NONE:
				परत none_;
			स्थिती DriveType::CONSTANT:
				परत constant_.size();
			स्थिती DriveType::WIRE:
				परत wire_.size();
			स्थिती DriveType::PORT:
				परत port_.size();
			स्थिती DriveType::MARKER:
				परत marker_.size();
			स्थिती DriveType::MULTIPLE:
				परत multiple_.size();
		}
		log_abort();
	}
};

ओळीत DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple स्थिरांक &bit)
	: width_(1)
{
	साठी (स्वतः स्थिरांक &bit : bit.multiple())
		multiple_.emplace(bit);
}

संरचना DriveSpec
{
खाजगी:
	पूर्णांक width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable अचिन्हित पूर्णांक hash_ = 0;
सार्वजनिक:

	ओळीत bool packed() स्थिरांक {
		परत bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk स्थिरांक &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire स्थिरांक &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort स्थिरांक &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker स्थिरांक &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple स्थिरांक &chunk) { *this = chunk; }

	DriveSpec(DriveBit स्थिरांक &bit) { *this = bit; }
	DriveSpec(DriveBitWire स्थिरांक &bit) { *this = bit; }
	DriveSpec(DriveBitPort स्थिरांक &bit) { *this = bit; }
	DriveSpec(DriveBitMarker स्थिरांक &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple स्थिरांक &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> स्थिरांक &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> स्थिरांक &bits)
	{
		साठी (स्वतः स्थिरांक &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec स्थिरांक &sig)
	{
		// TODO: converting one chunk at a time would be faster
		साठी (स्वतः स्थिरांक &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> स्थिरांक &chunks() स्थिरांक { pack(); परत chunks_; }
	std::vector<DriveBit> स्थिरांक &bits() स्थिरांक { unpack(); परत bits_; }

	पूर्णांक size() स्थिरांक { परत width_; }

	शून्य append(DriveBit स्थिरांक &bit);

	शून्य append(DriveChunk स्थिरांक &chunk);

	शून्य pack() स्थिरांक;

	शून्य unpack() स्थिरांक;

	DriveBit &चालक[](पूर्णांक index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		परत bits_[index];
	}

	स्थिरांक DriveBit &चालक[](पूर्णांक index) स्थिरांक
	{
		log_assert(index >= 0 && index < size());
		unpack();
		परत bits_[index];
	}

	शून्य clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &चालक=(DriveChunk स्थिरांक &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		परत *this;
	}

	DriveSpec &चालक=(DriveChunkWire स्थिरांक &chunk) { परत *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkPort स्थिरांक &chunk) { परत *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMarker स्थिरांक &chunk) { परत *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMultiple स्थिरांक &chunk) { परत *this = DriveChunk(chunk); }

	DriveSpec &चालक=(DriveBit स्थिरांक &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		परत *this;
	}

	DriveSpec &चालक=(DriveBitWire स्थिरांक &bit) { परत *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitPort स्थिरांक &bit) { परत *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMarker स्थिरांक &bit) { परत *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMultiple स्थिरांक &bit) { परत *this = DriveBit(bit); }

	शून्य updhash() स्थिरांक {
		जर (hash_ != 0)
			परत;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;

	bool चालक==(DriveSpec स्थिरांक &other) स्थिरांक {
		updhash();
		other.updhash();
		जर (size() != other.size() || hash_ != other.hash_)
			परत false;
		परत chunks() == other.chunks();
	}

खाजगी:
	शून्य compute_width();
};



संरचना DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

खाजगी:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	संरचना DriveBitId
	{
		पूर्णांक id = -1;

		DriveBitId() {};

		DriveBitId(पूर्णांक id) : id(id) { }

		bool चालक==(स्थिरांक DriveBitId &other) स्थिरांक { परत id == other.id; }
		bool चालक!=(स्थिरांक DriveBitId &other) स्थिरांक { परत id != other.id; }
		bool चालक<(स्थिरांक DriveBitId &other) स्थिरांक { परत id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	संरचना DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		शून्य add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId काढा_edge(DriveBitId src);
		शून्य clear(DriveBitId src);
		bool contains(DriveBitId src);
		पूर्णांक मोजणी(DriveBitId src);

		DriveBitId at(DriveBitId src, पूर्णांक index);
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
	पूर्णांक next_offset = 1 + (पूर्णांक)State::Sm;

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

	BitMode bit_mode(DriveBit स्थिरांक &bit);
	DriveBitId id_from_drive_bit(DriveBit स्थिरांक &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	शून्य connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	शून्य connect_undirected(DriveBitId a_id, DriveBitId b_id);

सार्वजनिक:

	शून्य add(Module *module);

	// Add a single bit connection to the driver map.
	शून्य add(DriveBit स्थिरांक &a, DriveBit स्थिरांक &b);

	ढाँचा<typename T>
	स्थिर constexpr bool is_sig_type() {
		परत
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
	ढाँचा<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T स्थिरांक &a, U स्थिरांक &b)
	{
		log_assert(a.size() == b.size());
		साठी (पूर्णांक i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	शून्य add(SigSpec स्थिरांक &a, SigSpec स्थिरांक &b);

खाजगी:
	शून्य add_port(Cell *cell, IdString port, SigSpec स्थिरांक &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, पूर्णांक> orient_undirected_distance;

	शून्य orient_undirected(DriveBitId id);

सार्वजनिक:
	DriveBit चालक()(DriveBit स्थिरांक &bit);

	DriveSpec चालक()(DriveSpec spec);

खाजगी:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		परत wire->has_attribute(ID(keep));
	}
};

ओळीत Hasher DriveBitWire::hash_into(Hasher h) स्थिरांक
{
	h.eat(wire->name);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveBitPort::hash_into(Hasher h) स्थिरांक
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveBitMarker::hash_into(Hasher h) स्थिरांक
{
	h.eat(marker);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveBitMultiple::hash_into(Hasher h) स्थिरांक
{
	h.eat(multiple_);
	परत h;
}

ओळीत Hasher DriveBit::hash_into(Hasher h) स्थिरांक
{
	निवड (type_) {
	स्थिती DriveType::NONE:
		h.eat(0);
		थांबा;
	स्थिती DriveType::CONSTANT:
		h.eat(constant_);
		थांबा;
	स्थिती DriveType::WIRE:
		h.eat(wire_);
		थांबा;
	स्थिती DriveType::PORT:
		h.eat(port_);
		थांबा;
	स्थिती DriveType::MARKER:
		h.eat(marker_);
		थांबा;
	स्थिती DriveType::MULTIPLE:
		h.eat(multiple_);
		थांबा;
	}
	h.eat(type_);
	परत h;
}

ओळीत Hasher DriveChunkWire::hash_into(Hasher h) स्थिरांक
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveChunkPort::hash_into(Hasher h) स्थिरांक
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveChunkMarker::hash_into(Hasher h) स्थिरांक
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	परत h;
}

ओळीत Hasher DriveChunkMultiple::hash_into(Hasher h) स्थिरांक
{
	h.eat(width_);
	h.eat(multiple_);
	परत h;
}

ओळीत Hasher DriveChunk::hash_into(Hasher h) स्थिरांक
{
	निवड (type_) {
	स्थिती DriveType::NONE:
		h.eat(0);
		थांबा;
	स्थिती DriveType::CONSTANT:
		h.eat(constant_);
		थांबा;
	स्थिती DriveType::WIRE:
		h.eat(wire_);
		थांबा;
	स्थिती DriveType::PORT:
		h.eat(port_);
		थांबा;
	स्थिती DriveType::MARKER:
		h.eat(marker_);
		थांबा;
	स्थिती DriveType::MULTIPLE:
		h.eat(multiple_);
		थांबा;
	}
	h.eat(type_);
	परत h;
}

ओळीत Hasher DriveSpec::hash_into(Hasher h) स्थिरांक
{
	updhash();
	h.eat(hash_);
	परत h;
}

ओळीत Hasher DriverMap::DriveBitId::hash_into(Hasher h) स्थिरांक
{
	h.eat(id);
	परत h;
}

YOSYS_NAMESPACE_END

#endif
