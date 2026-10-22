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

#अगर_अघोषित DRIVERTOOLS_H
#घोषणा DRIVERTOOLS_H

#समावेश <string>
#समावेश <type_traits>

#समावेश "kernel/rtlil.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

काष्ठा DriveBit;

काष्ठा DriveChunkWire;
काष्ठा DriveChunkPort;
काष्ठा DriveChunkMarker;
काष्ठा DriveChunk;

काष्ठा DriveSpec;

std::string log_signal(DriveChunkWire स्थिर &chunk);
std::string log_signal(DriveChunkPort स्थिर &chunk);
std::string log_signal(DriveChunkMarker स्थिर &chunk);
std::string log_signal(DriveChunk स्थिर &chunk);
std::string log_signal(DriveSpec स्थिर &chunk);

क्रमागत class DriveType : अचिन्हित अक्षर
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

काष्ठा DriveBitWire
{
	Wire *wire;
	पूर्णांक offset;

	DriveBitWire(Wire *wire, पूर्णांक offset) : wire(wire), offset(offset) {}

	bool चालक==(स्थिर DriveBitWire &other) स्थिर
	{
		वापस wire == other.wire && offset == other.offset;
	}

	bool चालक<(स्थिर DriveBitWire &other) स्थिर
	{
		अगर (wire != other.wire)
			वापस wire->name < other.wire->name;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;


	चालक SigBit() स्थिर
	{
		वापस SigBit(wire, offset);
	}
};

काष्ठा DriveBitPort
{
	Cell *cell;
	IdString port;
	पूर्णांक offset;

	DriveBitPort(Cell *cell, IdString port, पूर्णांक offset) : cell(cell), port(port), offset(offset) {}

	bool चालक==(स्थिर DriveBitPort &other) स्थिर
	{
		वापस cell == other.cell && port == other.port && offset == other.offset;
	}

	bool चालक<(स्थिर DriveBitPort &other) स्थिर
	{
		अगर (cell != other.cell)
			वापस cell->name < other.cell->name;
		अगर (port != other.port)
			वापस port < other.port;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

};


काष्ठा DriveBitMarker
{
	पूर्णांक marker;
	पूर्णांक offset;

	DriveBitMarker(पूर्णांक marker, पूर्णांक offset) : marker(marker), offset(offset) {}

	bool चालक==(स्थिर DriveBitMarker &other) स्थिर
	{
		वापस marker == other.marker && offset == other.offset;
	}

	bool चालक<(स्थिर DriveBitMarker &other) स्थिर
	{
		अगर (marker != other.marker)
			वापस marker < other.marker;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

};

काष्ठा DriveBitMultiple
{
निजी:
	pool<DriveBit> multiple_;

खुला:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit स्थिर &single);

	pool<DriveBit> स्थिर &multiple() स्थिर { वापस multiple_; }

	व्योम merge(DriveBitMultiple स्थिर &other)
	{
		क्रम (DriveBit स्थिर &single : other.multiple_)
			merge(single);
	}

	व्योम merge(DriveBitMultiple &&other)
	{
		क्रम (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	व्योम merge(DriveBit स्थिर &single);
	व्योम merge(DriveBit &&single);

	bool चालक==(स्थिर DriveBitMultiple &other) स्थिर
	{
		वापस multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

काष्ठा DriveBit
{
निजी:
	DriveType type_ = DriveType::NONE;
	जोड़
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
खुला:
	DriveBit()  {}

	DriveBit(SigBit स्थिर &bit);

	DriveBit(DriveBit स्थिर &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire स्थिर &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort स्थिर &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker स्थिर &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple स्थिर &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	व्योम set_none()
	{
		चयन (type_)
		{
			हाल DriveType::NONE:
				अवरोध;
			हाल DriveType::CONSTANT:
				अवरोध;
			हाल DriveType::WIRE:
				wire_.~DriveBitWire();
				अवरोध;
			हाल DriveType::PORT:
				port_.~DriveBitPort();
				अवरोध;
			हाल DriveType::MARKER:
				marker_.~DriveBitMarker();
				अवरोध;
			हाल DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				अवरोध;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &चालक=(DriveBit स्थिर &other)
	{
		चयन (other.type_)
		{
			हाल DriveType::NONE:
				set_none();
				अवरोध;
			हाल DriveType::CONSTANT:
				*this = other.constant_;
				अवरोध;
			हाल DriveType::WIRE:
				*this = other.wire_;
				अवरोध;
			हाल DriveType::PORT:
				*this = other.port_;
				अवरोध;
			हाल DriveType::MARKER:
				*this = other.marker_;
				अवरोध;
			हाल DriveType::MULTIPLE:
				*this = other.multiple_;
				अवरोध;
		}
		वापस *this;
	}

	DriveBit &चालक=(DriveBit &&other)
	{
		चयन (other.type_)
		{
			हाल DriveType::NONE:
				set_none();
				अवरोध;
			हाल DriveType::CONSTANT:
				*this = std::move(other.constant_);
				अवरोध;
			हाल DriveType::WIRE:
				*this = std::move(other.wire_);
				अवरोध;
			हाल DriveType::PORT:
				*this = std::move(other.port_);
				अवरोध;
			हाल DriveType::MARKER:
				*this = std::move(other.marker_);
				अवरोध;
			हाल DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				अवरोध;
		}
		वापस *this;
	}

	DriveBit &चालक=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitWire स्थिर &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitPort स्थिर &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitMarker स्थिर &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitMultiple स्थिर &multiple)
	{
		set_none();
		अगर (multiple.multiple().empty())
			वापस *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		वापस *this;
	}

	DriveBit &चालक=(DriveBitMultiple &&multiple)
	{
		set_none();
		अगर (multiple.multiple().empty())
			वापस *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		वापस *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

	bool चालक==(स्थिर DriveBit &other) स्थिर
	{
		अगर (type_ != other.type_)
			वापस false;

		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस true;
			हाल DriveType::CONSTANT:
				वापस constant_ == other.constant_;
			हाल DriveType::WIRE:
				वापस wire_ == other.wire_;
			हाल DriveType::PORT:
				वापस port_ == other.port_;
			हाल DriveType::MARKER:
				वापस marker_ == other.marker_;
			हाल DriveType::MULTIPLE:
				वापस multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(स्थिर DriveBit &other) स्थिर
	{
		वापस !(*this == other);
	}

	bool चालक<(स्थिर DriveBit &other) स्थिर
	{
		अगर (type_ != other.type_)
			वापस type_ < other.type_;
		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस false;
			हाल DriveType::CONSTANT:
				वापस constant_ < other.constant_;
			हाल DriveType::WIRE:
				वापस wire_ < other.wire_;
			हाल DriveType::PORT:
				वापस port_ < other.port_;
			हाल DriveType::MARKER:
				वापस marker_ < other.marker_;
			हाल DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() स्थिर { वापस type_; }

	bool is_none() स्थिर { वापस type_ == DriveType::NONE; }
	bool is_constant() स्थिर { वापस type_ == DriveType::CONSTANT; }
	bool is_wire() स्थिर { वापस type_ == DriveType::WIRE; }
	bool is_port() स्थिर { वापस type_ == DriveType::PORT; }
	bool is_marker() स्थिर { वापस type_ == DriveType::MARKER; }
	bool is_multiple() स्थिर { वापस type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); वापस constant_; }
	State स्थिर &constant() स्थिर { log_assert(is_constant()); वापस constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); वापस wire_; }
	DriveBitWire स्थिर &wire() स्थिर { log_assert(is_wire()); वापस wire_; }
	DriveBitPort &port() { log_assert(is_port()); वापस port_; }
	DriveBitPort स्थिर &port() स्थिर { log_assert(is_port()); वापस port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); वापस marker_; }
	DriveBitMarker स्थिर &marker() स्थिर { log_assert(is_marker()); वापस marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); वापस multiple_; }
	DriveBitMultiple स्थिर &multiple() स्थिर { log_assert(is_multiple()); वापस multiple_; }

	व्योम merge(DriveBit स्थिर &other);

};

अंतरभूत DriveBitMultiple::DriveBitMultiple() {}
अंतरभूत DriveBitMultiple::DriveBitMultiple(DriveBit स्थिर &single)
{
	multiple_.emplace(single);
}

काष्ठा DriveChunkWire
{
	Wire *wire;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkWire(Wire *wire, पूर्णांक offset, पूर्णांक width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire स्थिर &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	पूर्णांक आकार() स्थिर { वापस width; }

	DriveBitWire चालक[](पूर्णांक i) स्थिर
	{
		log_assert(i >= 0 && i < width);
		वापस DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire स्थिर &bit) स्थिर;
	bool try_append(DriveBitWire स्थिर &bit);
	bool try_append(DriveChunkWire स्थिर &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() स्थिर { वापस offset == 0 && width == wire->width; }

	bool चालक==(स्थिर DriveChunkWire &other) स्थिर
	{
		वापस wire == other.wire && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिर DriveChunkWire &other) स्थिर
	{
		अगर (wire != other.wire)
			वापस wire->name < other.wire->name;
		अगर (width != other.width)
			वापस width < other.width;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

	explicit चालक SigChunk() स्थिर
	{
		वापस SigChunk(wire, offset, width);
	}
};

काष्ठा DriveChunkPort
{
	Cell *cell;
	IdString port;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkPort(Cell *cell, IdString port, पूर्णांक offset, पूर्णांक width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(Get_आकार(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> स्थिर &conn) :
		cell(cell), port(conn.first), offset(0), width(Get_आकार(conn.second)) { }
	DriveChunkPort(DriveBitPort स्थिर &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	पूर्णांक आकार() स्थिर { वापस width; }

	DriveBitPort चालक[](पूर्णांक i) स्थिर
	{
		log_assert(i >= 0 && i < width);
		वापस DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort स्थिर &bit) स्थिर;
	bool try_append(DriveBitPort स्थिर &bit);
	bool try_append(DriveChunkPort स्थिर &chunk);

	// Whether this chunk is a whole port
	bool is_whole() स्थिर
	{
		वापस offset == 0 && width == cell->connections().at(port).आकार();
	}

	bool चालक==(स्थिर DriveChunkPort &other) स्थिर
	{
		वापस cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिर DriveChunkPort &other) स्थिर
	{
		अगर (cell != other.cell)
			वापस cell->name < other.cell->name;
		अगर (port != other.port)
			वापस port < other.port;
		अगर (width != other.width)
			वापस width < other.width;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};


काष्ठा DriveChunkMarker
{
	पूर्णांक marker;
	पूर्णांक offset;
	पूर्णांक width;

	DriveChunkMarker(पूर्णांक marker, पूर्णांक offset, पूर्णांक width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker स्थिर &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	पूर्णांक आकार() स्थिर { वापस width; }

	DriveBitMarker चालक[](पूर्णांक i) स्थिर
	{
		log_assert(i >= 0 && i < width);
		वापस DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker स्थिर &bit) स्थिर;
	bool try_append(DriveBitMarker स्थिर &bit);
	bool try_append(DriveChunkMarker स्थिर &chunk);

	bool चालक==(स्थिर DriveChunkMarker &other) स्थिर
	{
		वापस marker == other.marker && offset == other.offset && width == other.width;
	}

	bool चालक<(स्थिर DriveChunkMarker &other) स्थिर
	{
		अगर (marker != other.marker)
			वापस marker < other.marker;
		अगर (width != other.width)
			वापस width < other.width;
		वापस offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

काष्ठा DriveChunkMultiple
{
निजी:
	mutable pool<DriveChunk> multiple_;
	पूर्णांक width_;

खुला:
	pool<DriveChunk> स्थिर &multiple() स्थिर { वापस multiple_; }

	DriveChunkMultiple(DriveBitMultiple स्थिर &bit);

	पूर्णांक आकार() स्थिर { वापस width_; }

	DriveBitMultiple चालक[](पूर्णांक i) स्थिर;

	bool can_append(DriveBitMultiple स्थिर &bit) स्थिर;

	bool try_append(DriveBitMultiple स्थिर &bit);


	bool can_append(DriveChunkMultiple स्थिर &bit) स्थिर;

	bool try_append(DriveChunkMultiple स्थिर &bit);

	bool चालक==(स्थिर DriveChunkMultiple &other) स्थिर
	{
		वापस width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool चालक<(स्थिर DriveChunkMultiple &other) स्थिर
	{
		अगर (multiple_.आकार() < other.multiple_.आकार())

		multiple_.sort();
		वापस false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

काष्ठा DriveChunk
{
निजी:
	DriveType type_ = DriveType::NONE;
	जोड़
	{
		पूर्णांक none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

खुला:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk स्थिर &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit स्थिर &other) { *this = other; }

	DriveChunk(Const स्थिर &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire स्थिर &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort स्थिर &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker स्थिर &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple स्थिर &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit चालक[](पूर्णांक i) स्थिर
	{
		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस DriveBit();
			हाल DriveType::CONSTANT:
				वापस constant_[i];
			हाल DriveType::WIRE:
				वापस wire_[i];
			हाल DriveType::PORT:
				वापस port_[i];
			हाल DriveType::MARKER:
				वापस marker_[i];
			हाल DriveType::MULTIPLE:
				वापस multiple_[i];
		}
		log_abort();
	}

	व्योम set_none(पूर्णांक width = 0)
	{
		चयन (type_)
		{
			हाल DriveType::NONE:
				none_ = width;
				अवरोध;
			हाल DriveType::CONSTANT:
				constant_.~Const();
				अवरोध;
			हाल DriveType::WIRE:
				wire_.~DriveChunkWire();
				अवरोध;
			हाल DriveType::PORT:
				port_.~DriveChunkPort();
				अवरोध;
			हाल DriveType::MARKER:
				marker_.~DriveChunkMarker();
				अवरोध;
			हाल DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				अवरोध;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &चालक=(DriveChunk स्थिर &other)
	{
		चयन (other.type_)
		{
			हाल DriveType::NONE:
				set_none(other.none_);
				अवरोध;
			हाल DriveType::CONSTANT:
				*this = other.constant_;
				अवरोध;
			हाल DriveType::WIRE:
				*this = other.wire_;
				अवरोध;
			हाल DriveType::PORT:
				*this = other.port_;
				अवरोध;
			हाल DriveType::MARKER:
				*this = other.marker_;
				अवरोध;
			हाल DriveType::MULTIPLE:
				*this = other.multiple_;
				अवरोध;
		}
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunk &&other)
	{
		चयन (other.type_)
		{
			हाल DriveType::NONE:
				set_none(other.none_);
				अवरोध;
			हाल DriveType::CONSTANT:
				*this = std::move(other.constant_);
				अवरोध;
			हाल DriveType::WIRE:
				*this = std::move(other.wire_);
				अवरोध;
			हाल DriveType::PORT:
				*this = std::move(other.port_);
				अवरोध;
			हाल DriveType::MARKER:
				*this = std::move(other.marker_);
				अवरोध;
			हाल DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				अवरोध;
		}
		वापस *this;
	}

	DriveChunk &चालक=(Const स्थिर &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		वापस *this;
	}

	DriveChunk &चालक=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkWire स्थिर &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkPort स्थिर &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkMarker स्थिर &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple स्थिर &multiple)
	{
		set_none(multiple.आकार());
		अगर (multiple.multiple().empty())
			वापस *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		वापस *this;
	}

	DriveChunk &चालक=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.आकार());
		अगर (multiple.multiple().empty())
			वापस *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		वापस *this;
	}

	DriveChunk &चालक=(DriveBit स्थिर &other)
	{
		चयन (other.type())
		{
			हाल DriveType::NONE:
				set_none(1);
				अवरोध;
			हाल DriveType::CONSTANT:
				*this = Const(other.constant());
				अवरोध;
			हाल DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				अवरोध;
			हाल DriveType::PORT:
				*this = DriveChunkPort(other.port());
				अवरोध;
			हाल DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				अवरोध;
			हाल DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				अवरोध;
		}
		वापस *this;
	}

	bool can_append(DriveBit स्थिर &bit) स्थिर;
	bool try_append(DriveBit स्थिर &bit);
	bool try_append(DriveChunk स्थिर &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

	bool चालक==(स्थिर DriveChunk &other) स्थिर
	{
		अगर (type_ != other.type_)
			वापस false;

		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस true;
			हाल DriveType::CONSTANT:
				वापस constant_ == other.constant_;
			हाल DriveType::WIRE:
				वापस wire_ == other.wire_;
			हाल DriveType::PORT:
				वापस port_ == other.port_;
			हाल DriveType::MARKER:
				वापस marker_ == other.marker_;
			हाल DriveType::MULTIPLE:
				वापस multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool चालक!=(स्थिर DriveChunk &other) स्थिर
	{
		वापस !(*this == other);
	}

	bool चालक<(स्थिर DriveChunk &other) स्थिर
	{
		अगर (type_ != other.type_)
			वापस type_ < other.type_;
		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस false;
			हाल DriveType::CONSTANT:
				वापस constant_ < other.constant_;
			हाल DriveType::WIRE:
				वापस wire_ < other.wire_;
			हाल DriveType::PORT:
				वापस port_ < other.port_;
			हाल DriveType::MARKER:
				वापस marker_ < other.marker_;
			हाल DriveType::MULTIPLE:
				वापस multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() स्थिर { वापस type_; }

	bool is_none() स्थिर { वापस type_ == DriveType::NONE; }
	bool is_constant() स्थिर { वापस type_ == DriveType::CONSTANT; }
	bool is_wire() स्थिर { वापस type_ == DriveType::WIRE; }
	bool is_port() स्थिर { वापस type_ == DriveType::PORT; }
	bool is_marker() स्थिर { वापस type_ == DriveType::MARKER; }
	bool is_multiple() स्थिर { वापस type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); वापस constant_; }
	Const स्थिर &constant() स्थिर { log_assert(is_constant()); वापस constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); वापस wire_; }
	DriveChunkWire स्थिर &wire() स्थिर { log_assert(is_wire()); वापस wire_; }
	DriveChunkPort &port() { log_assert(is_port()); वापस port_; }
	DriveChunkPort स्थिर &port() स्थिर { log_assert(is_port()); वापस port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); वापस marker_; }
	DriveChunkMarker स्थिर &marker() स्थिर { log_assert(is_marker()); वापस marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); वापस multiple_; }
	DriveChunkMultiple स्थिर &multiple() स्थिर { log_assert(is_multiple()); वापस multiple_; }


	पूर्णांक आकार() स्थिर
	{
		चयन (type_)
		{
			हाल DriveType::NONE:
				वापस none_;
			हाल DriveType::CONSTANT:
				वापस constant_.आकार();
			हाल DriveType::WIRE:
				वापस wire_.आकार();
			हाल DriveType::PORT:
				वापस port_.आकार();
			हाल DriveType::MARKER:
				वापस marker_.आकार();
			हाल DriveType::MULTIPLE:
				वापस multiple_.आकार();
		}
		log_abort();
	}
};

अंतरभूत DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple स्थिर &bit)
	: width_(1)
{
	क्रम (स्वतः स्थिर &bit : bit.multiple())
		multiple_.emplace(bit);
}

काष्ठा DriveSpec
{
निजी:
	पूर्णांक width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable अचिन्हित पूर्णांक hash_ = 0;
खुला:

	अंतरभूत bool packed() स्थिर {
		वापस bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk स्थिर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire स्थिर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort स्थिर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker स्थिर &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple स्थिर &chunk) { *this = chunk; }

	DriveSpec(DriveBit स्थिर &bit) { *this = bit; }
	DriveSpec(DriveBitWire स्थिर &bit) { *this = bit; }
	DriveSpec(DriveBitPort स्थिर &bit) { *this = bit; }
	DriveSpec(DriveBitMarker स्थिर &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple स्थिर &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> स्थिर &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> स्थिर &bits)
	{
		क्रम (स्वतः स्थिर &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec स्थिर &sig)
	{
		// TODO: converting one chunk at a time would be faster
		क्रम (स्वतः स्थिर &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> स्थिर &chunks() स्थिर { pack(); वापस chunks_; }
	std::vector<DriveBit> स्थिर &bits() स्थिर { unpack(); वापस bits_; }

	पूर्णांक आकार() स्थिर { वापस width_; }

	व्योम append(DriveBit स्थिर &bit);

	व्योम append(DriveChunk स्थिर &chunk);

	व्योम pack() स्थिर;

	व्योम unpack() स्थिर;

	DriveBit &चालक[](पूर्णांक सूचकांक)
	{
		log_assert(सूचकांक >= 0 && सूचकांक < आकार());
		unpack();
		वापस bits_[सूचकांक];
	}

	स्थिर DriveBit &चालक[](पूर्णांक सूचकांक) स्थिर
	{
		log_assert(सूचकांक >= 0 && सूचकांक < आकार());
		unpack();
		वापस bits_[सूचकांक];
	}

	व्योम clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &चालक=(DriveChunk स्थिर &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		वापस *this;
	}

	DriveSpec &चालक=(DriveChunkWire स्थिर &chunk) { वापस *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkPort स्थिर &chunk) { वापस *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMarker स्थिर &chunk) { वापस *this = DriveChunk(chunk); }
	DriveSpec &चालक=(DriveChunkMultiple स्थिर &chunk) { वापस *this = DriveChunk(chunk); }

	DriveSpec &चालक=(DriveBit स्थिर &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		वापस *this;
	}

	DriveSpec &चालक=(DriveBitWire स्थिर &bit) { वापस *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitPort स्थिर &bit) { वापस *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMarker स्थिर &bit) { वापस *this = DriveBit(bit); }
	DriveSpec &चालक=(DriveBitMultiple स्थिर &bit) { वापस *this = DriveBit(bit); }

	व्योम updhash() स्थिर {
		अगर (hash_ != 0)
			वापस;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;

	bool चालक==(DriveSpec स्थिर &other) स्थिर {
		updhash();
		other.updhash();
		अगर (आकार() != other.आकार() || hash_ != other.hash_)
			वापस false;
		वापस chunks() == other.chunks();
	}

निजी:
	व्योम compute_width();
};



काष्ठा DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

निजी:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	काष्ठा DriveBitId
	{
		पूर्णांक id = -1;

		DriveBitId() {};

		DriveBitId(पूर्णांक id) : id(id) { }

		bool चालक==(स्थिर DriveBitId &other) स्थिर { वापस id == other.id; }
		bool चालक!=(स्थिर DriveBitId &other) स्थिर { वापस id != other.id; }
		bool चालक<(स्थिर DriveBitId &other) स्थिर { वापस id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	काष्ठा DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		व्योम add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId निकालो_edge(DriveBitId src);
		व्योम clear(DriveBitId src);
		bool contains(DriveBitId src);
		पूर्णांक गिनती(DriveBitId src);

		DriveBitId at(DriveBitId src, पूर्णांक सूचकांक);
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
	पूर्णांक अग्र_offset = 1 + (पूर्णांक)State::Sm;

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


	क्रमागत class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit स्थिर &bit);
	DriveBitId id_from_drive_bit(DriveBit स्थिर &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	व्योम connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	व्योम connect_directed_बफर(DriveBitId driven_id, DriveBitId driver_id);
	व्योम connect_undirected(DriveBitId a_id, DriveBitId b_id);

खुला:

	व्योम add(Module *module);

	// Add a single bit connection to the driver map.
	व्योम add(DriveBit स्थिर &a, DriveBit स्थिर &b);

	ढाँचा<typename T>
	अटल constexpr bool is_sig_type() {
		वापस
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
	add(T स्थिर &a, U स्थिर &b)
	{
		log_assert(a.आकार() == b.आकार());
		क्रम (पूर्णांक i = 0; i != Get_आकार(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	व्योम add(SigSpec स्थिर &a, SigSpec स्थिर &b);

निजी:
	व्योम add_port(Cell *cell, IdString port, SigSpec स्थिर &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, पूर्णांक> orient_undirected_distance;

	व्योम orient_undirected(DriveBitId id);

खुला:
	DriveBit चालक()(DriveBit स्थिर &bit);

	DriveSpec चालक()(DriveSpec spec);

निजी:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		वापस wire->has_attribute(ID(keep));
	}
};

अंतरभूत Hasher DriveBitWire::hash_into(Hasher h) स्थिर
{
	h.eat(wire->name);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveBitPort::hash_into(Hasher h) स्थिर
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveBitMarker::hash_into(Hasher h) स्थिर
{
	h.eat(marker);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveBitMultiple::hash_into(Hasher h) स्थिर
{
	h.eat(multiple_);
	वापस h;
}

अंतरभूत Hasher DriveBit::hash_into(Hasher h) स्थिर
{
	चयन (type_) {
	हाल DriveType::NONE:
		h.eat(0);
		अवरोध;
	हाल DriveType::CONSTANT:
		h.eat(constant_);
		अवरोध;
	हाल DriveType::WIRE:
		h.eat(wire_);
		अवरोध;
	हाल DriveType::PORT:
		h.eat(port_);
		अवरोध;
	हाल DriveType::MARKER:
		h.eat(marker_);
		अवरोध;
	हाल DriveType::MULTIPLE:
		h.eat(multiple_);
		अवरोध;
	}
	h.eat(type_);
	वापस h;
}

अंतरभूत Hasher DriveChunkWire::hash_into(Hasher h) स्थिर
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveChunkPort::hash_into(Hasher h) स्थिर
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveChunkMarker::hash_into(Hasher h) स्थिर
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	वापस h;
}

अंतरभूत Hasher DriveChunkMultiple::hash_into(Hasher h) स्थिर
{
	h.eat(width_);
	h.eat(multiple_);
	वापस h;
}

अंतरभूत Hasher DriveChunk::hash_into(Hasher h) स्थिर
{
	चयन (type_) {
	हाल DriveType::NONE:
		h.eat(0);
		अवरोध;
	हाल DriveType::CONSTANT:
		h.eat(constant_);
		अवरोध;
	हाल DriveType::WIRE:
		h.eat(wire_);
		अवरोध;
	हाल DriveType::PORT:
		h.eat(port_);
		अवरोध;
	हाल DriveType::MARKER:
		h.eat(marker_);
		अवरोध;
	हाल DriveType::MULTIPLE:
		h.eat(multiple_);
		अवरोध;
	}
	h.eat(type_);
	वापस h;
}

अंतरभूत Hasher DriveSpec::hash_into(Hasher h) स्थिर
{
	updhash();
	h.eat(hash_);
	वापस h;
}

अंतरभूत Hasher DriverMap::DriveBitId::hash_into(Hasher h) स्थिर
{
	h.eat(id);
	वापस h;
}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
