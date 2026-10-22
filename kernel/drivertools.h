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

#അന്തര്ഭാവ <string>
#അന്തര്ഭാവ <type_traits>

#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ഘടന DriveBit;

ഘടന DriveChunkWire;
ഘടന DriveChunkPort;
ഘടന DriveChunkMarker;
ഘടന DriveChunk;

ഘടന DriveSpec;

std::string log_signal(DriveChunkWire സ്ഥിരം &chunk);
std::string log_signal(DriveChunkPort സ്ഥിരം &chunk);
std::string log_signal(DriveChunkMarker സ്ഥിരം &chunk);
std::string log_signal(DriveChunk സ്ഥിരം &chunk);
std::string log_signal(DriveSpec സ്ഥിരം &chunk);

എണ്ണൽ class DriveType : unsigned അക്ഷരം
{
	NONE,
	CONSTANT,
	WIRE,
	PORT,
	MULTIPLE,
	MARKER,
};

ഘടന DriveBitWire
{
	Wire *wire;
	പൂർണ്ണസംഖ്യ offset;

	DriveBitWire(Wire *wire, പൂർണ്ണസംഖ്യ offset) : wire(wire), offset(offset) {}

	bool operator==(സ്ഥിരം DriveBitWire &other) സ്ഥിരം
	{
		തിരികെ wire == other.wire && offset == other.offset;
	}

	bool operator<(സ്ഥിരം DriveBitWire &other) സ്ഥിരം
	{
		എങ്കിൽ (wire != other.wire)
			തിരികെ wire->name < other.wire->name;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;


	operator SigBit() സ്ഥിരം
	{
		തിരികെ SigBit(wire, offset);
	}
};

ഘടന DriveBitPort
{
	Cell *cell;
	IdString port;
	പൂർണ്ണസംഖ്യ offset;

	DriveBitPort(Cell *cell, IdString port, പൂർണ്ണസംഖ്യ offset) : cell(cell), port(port), offset(offset) {}

	bool operator==(സ്ഥിരം DriveBitPort &other) സ്ഥിരം
	{
		തിരികെ cell == other.cell && port == other.port && offset == other.offset;
	}

	bool operator<(സ്ഥിരം DriveBitPort &other) സ്ഥിരം
	{
		എങ്കിൽ (cell != other.cell)
			തിരികെ cell->name < other.cell->name;
		എങ്കിൽ (port != other.port)
			തിരികെ port < other.port;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

};


ഘടന DriveBitMarker
{
	പൂർണ്ണസംഖ്യ marker;
	പൂർണ്ണസംഖ്യ offset;

	DriveBitMarker(പൂർണ്ണസംഖ്യ marker, പൂർണ്ണസംഖ്യ offset) : marker(marker), offset(offset) {}

	bool operator==(സ്ഥിരം DriveBitMarker &other) സ്ഥിരം
	{
		തിരികെ marker == other.marker && offset == other.offset;
	}

	bool operator<(സ്ഥിരം DriveBitMarker &other) സ്ഥിരം
	{
		എങ്കിൽ (marker != other.marker)
			തിരികെ marker < other.marker;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

};

ഘടന DriveBitMultiple
{
സ്വകാര്യ:
	pool<DriveBit> multiple_;

പൊതു:
	DriveBitMultiple();
	DriveBitMultiple(DriveBit സ്ഥിരം &single);

	pool<DriveBit> സ്ഥിരം &multiple() സ്ഥിരം { തിരികെ multiple_; }

	ശൂന്യം merge(DriveBitMultiple സ്ഥിരം &other)
	{
		വേണ്ടി (DriveBit സ്ഥിരം &single : other.multiple_)
			merge(single);
	}

	ശൂന്യം merge(DriveBitMultiple &&other)
	{
		വേണ്ടി (DriveBit &single : other.multiple_)
			merge(std::move(single));
	}

	ശൂന്യം merge(DriveBit സ്ഥിരം &single);
	ശൂന്യം merge(DriveBit &&single);

	bool operator==(സ്ഥിരം DriveBitMultiple &other) സ്ഥിരം
	{
		തിരികെ multiple_ == other.multiple_;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

ഘടന DriveBit
{
സ്വകാര്യ:
	DriveType type_ = DriveType::NONE;
	സംഘം
	{
		State constant_;
		DriveBitWire wire_;
		DriveBitPort port_;
		DriveBitMarker marker_;
		DriveBitMultiple multiple_;
	};
പൊതു:
	DriveBit()  {}

	DriveBit(SigBit സ്ഥിരം &bit);

	DriveBit(DriveBit സ്ഥിരം &other) { *this = other; }
	DriveBit(DriveBit &&other) { *this = other; }


	DriveBit(State constant) { *this = constant; }
	DriveBit(DriveBitWire സ്ഥിരം &wire) { *this = wire; }
	DriveBit(DriveBitWire &&wire) { *this = wire; }
	DriveBit(DriveBitPort സ്ഥിരം &port) { *this = port; }
	DriveBit(DriveBitPort &&port) { *this = port; }
	DriveBit(DriveBitMarker സ്ഥിരം &marker) { *this = marker; }
	DriveBit(DriveBitMarker &&marker) { *this = marker; }
	DriveBit(DriveBitMultiple സ്ഥിരം &multiple) { *this = multiple; }
	DriveBit(DriveBitMultiple &&multiple) { *this = multiple; }

	~DriveBit() { set_none(); }

	ശൂന്യം set_none()
	{
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				wire_.~DriveBitWire();
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				port_.~DriveBitPort();
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				marker_.~DriveBitMarker();
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				multiple_.~DriveBitMultiple();
				നിർത്തുക;
		}
		type_ = DriveType::NONE;
	}

	DriveBit &operator=(DriveBit സ്ഥിരം &other)
	{
		തിരഞ്ഞെടുപ്പ് (other.type_)
		{
			അവസ്ഥ DriveType::NONE:
				set_none();
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				*this = other.constant_;
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				*this = other.wire_;
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				*this = other.port_;
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				*this = other.marker_;
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				*this = other.multiple_;
				നിർത്തുക;
		}
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBit &&other)
	{
		തിരഞ്ഞെടുപ്പ് (other.type_)
		{
			അവസ്ഥ DriveType::NONE:
				set_none();
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				*this = std::move(other.wire_);
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				*this = std::move(other.port_);
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				*this = std::move(other.marker_);
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				നിർത്തുക;
		}
		തിരികെ *this;
	}

	DriveBit &operator=(State constant)
	{
		set_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitWire സ്ഥിരം &wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitWire &&wire)
	{
		set_none();
		new (&wire_) DriveBitWire(wire);
		type_ = DriveType::WIRE;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitPort സ്ഥിരം &port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitPort &&port)
	{
		set_none();
		new (&port_) DriveBitPort(port);
		type_ = DriveType::PORT;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitMarker സ്ഥിരം &marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitMarker &&marker)
	{
		set_none();
		new (&marker_) DriveBitMarker(marker);
		type_ = DriveType::MARKER;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitMultiple സ്ഥിരം &multiple)
	{
		set_none();
		എങ്കിൽ (multiple.multiple().empty())
			തിരികെ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		തിരികെ *this;
	}

	DriveBit &operator=(DriveBitMultiple &&multiple)
	{
		set_none();
		എങ്കിൽ (multiple.multiple().empty())
			തിരികെ *this;
		new (&multiple_) DriveBitMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		തിരികെ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

	bool operator==(സ്ഥിരം DriveBit &other) സ്ഥിരം
	{
		എങ്കിൽ (type_ != other.type_)
			തിരികെ false;

		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ true;
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_ == other.constant_;
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_ == other.wire_;
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_ == other.port_;
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_ == other.marker_;
			അവസ്ഥ DriveType::MULTIPLE:
				തിരികെ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(സ്ഥിരം DriveBit &other) സ്ഥിരം
	{
		തിരികെ !(*this == other);
	}

	bool operator<(സ്ഥിരം DriveBit &other) സ്ഥിരം
	{
		എങ്കിൽ (type_ != other.type_)
			തിരികെ type_ < other.type_;
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ false;
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_ < other.constant_;
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_ < other.wire_;
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_ < other.port_;
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_ < other.marker_;
			അവസ്ഥ DriveType::MULTIPLE:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() സ്ഥിരം { തിരികെ type_; }

	bool is_none() സ്ഥിരം { തിരികെ type_ == DriveType::NONE; }
	bool is_constant() സ്ഥിരം { തിരികെ type_ == DriveType::CONSTANT; }
	bool is_wire() സ്ഥിരം { തിരികെ type_ == DriveType::WIRE; }
	bool is_port() സ്ഥിരം { തിരികെ type_ == DriveType::PORT; }
	bool is_marker() സ്ഥിരം { തിരികെ type_ == DriveType::MARKER; }
	bool is_multiple() സ്ഥിരം { തിരികെ type_ == DriveType::MULTIPLE; }

	State &constant() { log_assert(is_constant()); തിരികെ constant_; }
	State സ്ഥിരം &constant() സ്ഥിരം { log_assert(is_constant()); തിരികെ constant_; }
	DriveBitWire &wire() { log_assert(is_wire()); തിരികെ wire_; }
	DriveBitWire സ്ഥിരം &wire() സ്ഥിരം { log_assert(is_wire()); തിരികെ wire_; }
	DriveBitPort &port() { log_assert(is_port()); തിരികെ port_; }
	DriveBitPort സ്ഥിരം &port() സ്ഥിരം { log_assert(is_port()); തിരികെ port_; }
	DriveBitMarker &marker() { log_assert(is_marker()); തിരികെ marker_; }
	DriveBitMarker സ്ഥിരം &marker() സ്ഥിരം { log_assert(is_marker()); തിരികെ marker_; }
	DriveBitMultiple &multiple() { log_assert(is_multiple()); തിരികെ multiple_; }
	DriveBitMultiple സ്ഥിരം &multiple() സ്ഥിരം { log_assert(is_multiple()); തിരികെ multiple_; }

	ശൂന്യം merge(DriveBit സ്ഥിരം &other);

};

വരിയിൽ DriveBitMultiple::DriveBitMultiple() {}
വരിയിൽ DriveBitMultiple::DriveBitMultiple(DriveBit സ്ഥിരം &single)
{
	multiple_.emplace(single);
}

ഘടന DriveChunkWire
{
	Wire *wire;
	പൂർണ്ണസംഖ്യ offset;
	പൂർണ്ണസംഖ്യ width;

	DriveChunkWire(Wire *wire, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width) : wire(wire), offset(offset), width(width) {}
	DriveChunkWire(DriveBitWire സ്ഥിരം &bit) : wire(bit.wire), offset(bit.offset), width(1) {}

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width; }

	DriveBitWire operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം
	{
		log_assert(i >= 0 && i < width);
		തിരികെ DriveBitWire(wire, offset + i);
	}

	bool can_append(DriveBitWire സ്ഥിരം &bit) സ്ഥിരം;
	bool try_append(DriveBitWire സ്ഥിരം &bit);
	bool try_append(DriveChunkWire സ്ഥിരം &chunk);

	// Whether this chunk is a whole wire
	bool is_whole() സ്ഥിരം { തിരികെ offset == 0 && width == wire->width; }

	bool operator==(സ്ഥിരം DriveChunkWire &other) സ്ഥിരം
	{
		തിരികെ wire == other.wire && offset == other.offset && width == other.width;
	}

	bool operator<(സ്ഥിരം DriveChunkWire &other) സ്ഥിരം
	{
		എങ്കിൽ (wire != other.wire)
			തിരികെ wire->name < other.wire->name;
		എങ്കിൽ (width != other.width)
			തിരികെ width < other.width;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

	explicit operator SigChunk() സ്ഥിരം
	{
		തിരികെ SigChunk(wire, offset, width);
	}
};

ഘടന DriveChunkPort
{
	Cell *cell;
	IdString port;
	പൂർണ്ണസംഖ്യ offset;
	പൂർണ്ണസംഖ്യ width;

	DriveChunkPort(Cell *cell, IdString port, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width) :
		cell(cell), port(port), offset(offset), width(width) { }
	DriveChunkPort(Cell *cell, IdString port) :
		cell(cell), port(port), offset(0), width(GetSize(cell->connections().at(port))) { }
	DriveChunkPort(Cell *cell, std::pair<IdString, SigSpec> സ്ഥിരം &conn) :
		cell(cell), port(conn.first), offset(0), width(GetSize(conn.second)) { }
	DriveChunkPort(DriveBitPort സ്ഥിരം &bit) :
		cell(bit.cell), port(bit.port), offset(bit.offset), width(1) { }

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width; }

	DriveBitPort operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം
	{
		log_assert(i >= 0 && i < width);
		തിരികെ DriveBitPort(cell, port, offset + i);
	}

	bool can_append(DriveBitPort സ്ഥിരം &bit) സ്ഥിരം;
	bool try_append(DriveBitPort സ്ഥിരം &bit);
	bool try_append(DriveChunkPort സ്ഥിരം &chunk);

	// Whether this chunk is a whole port
	bool is_whole() സ്ഥിരം
	{
		തിരികെ offset == 0 && width == cell->connections().at(port).size();
	}

	bool operator==(സ്ഥിരം DriveChunkPort &other) സ്ഥിരം
	{
		തിരികെ cell == other.cell && port == other.port && offset == other.offset && width == other.width;
	}

	bool operator<(സ്ഥിരം DriveChunkPort &other) സ്ഥിരം
	{
		എങ്കിൽ (cell != other.cell)
			തിരികെ cell->name < other.cell->name;
		എങ്കിൽ (port != other.port)
			തിരികെ port < other.port;
		എങ്കിൽ (width != other.width)
			തിരികെ width < other.width;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};


ഘടന DriveChunkMarker
{
	പൂർണ്ണസംഖ്യ marker;
	പൂർണ്ണസംഖ്യ offset;
	പൂർണ്ണസംഖ്യ width;

	DriveChunkMarker(പൂർണ്ണസംഖ്യ marker, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width) :
		marker(marker), offset(offset), width(width) {}
	DriveChunkMarker(DriveBitMarker സ്ഥിരം &bit) :
		marker(bit.marker), offset(bit.offset), width(1) {}

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width; }

	DriveBitMarker operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം
	{
		log_assert(i >= 0 && i < width);
		തിരികെ DriveBitMarker(marker, offset + i);
	}

	bool can_append(DriveBitMarker സ്ഥിരം &bit) സ്ഥിരം;
	bool try_append(DriveBitMarker സ്ഥിരം &bit);
	bool try_append(DriveChunkMarker സ്ഥിരം &chunk);

	bool operator==(സ്ഥിരം DriveChunkMarker &other) സ്ഥിരം
	{
		തിരികെ marker == other.marker && offset == other.offset && width == other.width;
	}

	bool operator<(സ്ഥിരം DriveChunkMarker &other) സ്ഥിരം
	{
		എങ്കിൽ (marker != other.marker)
			തിരികെ marker < other.marker;
		എങ്കിൽ (width != other.width)
			തിരികെ width < other.width;
		തിരികെ offset < other.offset;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

ഘടന DriveChunkMultiple
{
സ്വകാര്യ:
	mutable pool<DriveChunk> multiple_;
	പൂർണ്ണസംഖ്യ width_;

പൊതു:
	pool<DriveChunk> സ്ഥിരം &multiple() സ്ഥിരം { തിരികെ multiple_; }

	DriveChunkMultiple(DriveBitMultiple സ്ഥിരം &bit);

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width_; }

	DriveBitMultiple operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം;

	bool can_append(DriveBitMultiple സ്ഥിരം &bit) സ്ഥിരം;

	bool try_append(DriveBitMultiple സ്ഥിരം &bit);


	bool can_append(DriveChunkMultiple സ്ഥിരം &bit) സ്ഥിരം;

	bool try_append(DriveChunkMultiple സ്ഥിരം &bit);

	bool operator==(സ്ഥിരം DriveChunkMultiple &other) സ്ഥിരം
	{
		തിരികെ width_ == other.width_ && multiple_ == other.multiple_;
	}

	bool operator<(സ്ഥിരം DriveChunkMultiple &other) സ്ഥിരം
	{
		എങ്കിൽ (multiple_.size() < other.multiple_.size())

		multiple_.sort();
		തിരികെ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
};

ഘടന DriveChunk
{
സ്വകാര്യ:
	DriveType type_ = DriveType::NONE;
	സംഘം
	{
		പൂർണ്ണസംഖ്യ none_;
		Const constant_;
		DriveChunkWire wire_;
		DriveChunkPort port_;
		DriveChunkMarker marker_;
		DriveChunkMultiple multiple_;
	};

പൊതു:
	DriveChunk() { set_none(); }

	DriveChunk(DriveChunk സ്ഥിരം &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(DriveBit സ്ഥിരം &other) { *this = other; }

	DriveChunk(Const സ്ഥിരം &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire സ്ഥിരം &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(DriveChunkPort സ്ഥിരം &port) { *this = port; }
	DriveChunk(DriveChunkPort &&port) { *this = port; }
	DriveChunk(DriveChunkMarker സ്ഥിരം &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(DriveChunkMultiple സ്ഥിരം &multiple) { *this = multiple; }
	DriveChunk(DriveChunkMultiple &&multiple) { *this = multiple; }

	~DriveChunk() { set_none(); }

	DriveBit operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം
	{
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ DriveBit();
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_[i];
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_[i];
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_[i];
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_[i];
			അവസ്ഥ DriveType::MULTIPLE:
				തിരികെ multiple_[i];
		}
		log_abort();
	}

	ശൂന്യം set_none(പൂർണ്ണസംഖ്യ width = 0)
	{
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				none_ = width;
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				constant_.~Const();
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				wire_.~DriveChunkWire();
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				port_.~DriveChunkPort();
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				multiple_.~DriveChunkMultiple();
				നിർത്തുക;
		}
		type_ = DriveType::NONE;
		none_ = width;
	}

	DriveChunk &operator=(DriveChunk സ്ഥിരം &other)
	{
		തിരഞ്ഞെടുപ്പ് (other.type_)
		{
			അവസ്ഥ DriveType::NONE:
				set_none(other.none_);
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				*this = other.constant_;
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				*this = other.wire_;
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				*this = other.port_;
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				*this = other.marker_;
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				*this = other.multiple_;
				നിർത്തുക;
		}
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunk &&other)
	{
		തിരഞ്ഞെടുപ്പ് (other.type_)
		{
			അവസ്ഥ DriveType::NONE:
				set_none(other.none_);
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				*this = std::move(other.wire_);
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				*this = std::move(other.port_);
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				*this = std::move(other.marker_);
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				*this = std::move(other.multiple_);
				നിർത്തുക;
		}
		തിരികെ *this;
	}

	DriveChunk &operator=(Const സ്ഥിരം &constant)
	{
		set_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		തിരികെ *this;
	}

	DriveChunk &operator=(Const &&constant)
	{
		set_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkWire സ്ഥിരം &wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkWire &&wire)
	{
		set_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkPort സ്ഥിരം &port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkPort &&port)
	{
		set_none();
		new (&port_) DriveChunkPort(port);
		type_ = DriveType::PORT;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkMarker സ്ഥിരം &marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkMarker &&marker)
	{
		set_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple സ്ഥിരം &multiple)
	{
		set_none(multiple.size());
		എങ്കിൽ (multiple.multiple().empty())
			തിരികെ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveChunkMultiple &&multiple)
	{
		set_none(multiple.size());
		എങ്കിൽ (multiple.multiple().empty())
			തിരികെ *this;
		new (&multiple_) DriveChunkMultiple(multiple);
		type_ = DriveType::MULTIPLE;
		തിരികെ *this;
	}

	DriveChunk &operator=(DriveBit സ്ഥിരം &other)
	{
		തിരഞ്ഞെടുപ്പ് (other.type())
		{
			അവസ്ഥ DriveType::NONE:
				set_none(1);
				നിർത്തുക;
			അവസ്ഥ DriveType::CONSTANT:
				*this = Const(other.constant());
				നിർത്തുക;
			അവസ്ഥ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				നിർത്തുക;
			അവസ്ഥ DriveType::PORT:
				*this = DriveChunkPort(other.port());
				നിർത്തുക;
			അവസ്ഥ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				നിർത്തുക;
			അവസ്ഥ DriveType::MULTIPLE:
				*this = DriveChunkMultiple(other.multiple());
				നിർത്തുക;
		}
		തിരികെ *this;
	}

	bool can_append(DriveBit സ്ഥിരം &bit) സ്ഥിരം;
	bool try_append(DriveBit സ്ഥിരം &bit);
	bool try_append(DriveChunk സ്ഥിരം &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

	bool operator==(സ്ഥിരം DriveChunk &other) സ്ഥിരം
	{
		എങ്കിൽ (type_ != other.type_)
			തിരികെ false;

		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ true;
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_ == other.constant_;
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_ == other.wire_;
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_ == other.port_;
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_ == other.marker_;
			അവസ്ഥ DriveType::MULTIPLE:
				തിരികെ multiple_ == other.multiple_;
		}
		log_abort();
	}

	bool operator!=(സ്ഥിരം DriveChunk &other) സ്ഥിരം
	{
		തിരികെ !(*this == other);
	}

	bool operator<(സ്ഥിരം DriveChunk &other) സ്ഥിരം
	{
		എങ്കിൽ (type_ != other.type_)
			തിരികെ type_ < other.type_;
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ false;
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_ < other.constant_;
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_ < other.wire_;
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_ < other.port_;
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_ < other.marker_;
			അവസ്ഥ DriveType::MULTIPLE:
				തിരികെ multiple_ < other.multiple_;
		}
		log_abort();
	}

	DriveType type() സ്ഥിരം { തിരികെ type_; }

	bool is_none() സ്ഥിരം { തിരികെ type_ == DriveType::NONE; }
	bool is_constant() സ്ഥിരം { തിരികെ type_ == DriveType::CONSTANT; }
	bool is_wire() സ്ഥിരം { തിരികെ type_ == DriveType::WIRE; }
	bool is_port() സ്ഥിരം { തിരികെ type_ == DriveType::PORT; }
	bool is_marker() സ്ഥിരം { തിരികെ type_ == DriveType::MARKER; }
	bool is_multiple() സ്ഥിരം { തിരികെ type_ == DriveType::MULTIPLE; }

	Const &constant() { log_assert(is_constant()); തിരികെ constant_; }
	Const സ്ഥിരം &constant() സ്ഥിരം { log_assert(is_constant()); തിരികെ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); തിരികെ wire_; }
	DriveChunkWire സ്ഥിരം &wire() സ്ഥിരം { log_assert(is_wire()); തിരികെ wire_; }
	DriveChunkPort &port() { log_assert(is_port()); തിരികെ port_; }
	DriveChunkPort സ്ഥിരം &port() സ്ഥിരം { log_assert(is_port()); തിരികെ port_; }
	DriveChunkMarker &marker() { log_assert(is_marker()); തിരികെ marker_; }
	DriveChunkMarker സ്ഥിരം &marker() സ്ഥിരം { log_assert(is_marker()); തിരികെ marker_; }
	DriveChunkMultiple &multiple() { log_assert(is_multiple()); തിരികെ multiple_; }
	DriveChunkMultiple സ്ഥിരം &multiple() സ്ഥിരം { log_assert(is_multiple()); തിരികെ multiple_; }


	പൂർണ്ണസംഖ്യ size() സ്ഥിരം
	{
		തിരഞ്ഞെടുപ്പ് (type_)
		{
			അവസ്ഥ DriveType::NONE:
				തിരികെ none_;
			അവസ്ഥ DriveType::CONSTANT:
				തിരികെ constant_.size();
			അവസ്ഥ DriveType::WIRE:
				തിരികെ wire_.size();
			അവസ്ഥ DriveType::PORT:
				തിരികെ port_.size();
			അവസ്ഥ DriveType::MARKER:
				തിരികെ marker_.size();
			അവസ്ഥ DriveType::MULTIPLE:
				തിരികെ multiple_.size();
		}
		log_abort();
	}
};

വരിയിൽ DriveChunkMultiple::DriveChunkMultiple(DriveBitMultiple സ്ഥിരം &bit)
	: width_(1)
{
	വേണ്ടി (auto സ്ഥിരം &bit : bit.multiple())
		multiple_.emplace(bit);
}

ഘടന DriveSpec
{
സ്വകാര്യ:
	പൂർണ്ണസംഖ്യ width_ = 0;
	mutable std::vector<DriveChunk> chunks_;
	mutable std::vector<DriveBit> bits_;
	mutable unsigned പൂർണ്ണസംഖ്യ hash_ = 0;
പൊതു:

	വരിയിൽ bool packed() സ്ഥിരം {
		തിരികെ bits_.empty();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk സ്ഥിരം &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire സ്ഥിരം &chunk) { *this = chunk; }
	DriveSpec(DriveChunkPort സ്ഥിരം &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker സ്ഥിരം &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMultiple സ്ഥിരം &chunk) { *this = chunk; }

	DriveSpec(DriveBit സ്ഥിരം &bit) { *this = bit; }
	DriveSpec(DriveBitWire സ്ഥിരം &bit) { *this = bit; }
	DriveSpec(DriveBitPort സ്ഥിരം &bit) { *this = bit; }
	DriveSpec(DriveBitMarker സ്ഥിരം &bit) { *this = bit; }
	DriveSpec(DriveBitMultiple സ്ഥിരം &bit) { *this = bit; }

	DriveSpec(std::vector<DriveChunk> സ്ഥിരം &chunks) : chunks_(chunks) { compute_width(); }

	DriveSpec(std::vector<DriveBit> സ്ഥിരം &bits)
	{
		വേണ്ടി (auto സ്ഥിരം &bit : bits)
			append(bit);
	}

	DriveSpec(SigSpec സ്ഥിരം &sig)
	{
		// TODO: converting one chunk at a time would be faster
		വേണ്ടി (auto സ്ഥിരം &bit : sig.bits())
			append(bit);
	}

	std::vector<DriveChunk> സ്ഥിരം &chunks() സ്ഥിരം { pack(); തിരികെ chunks_; }
	std::vector<DriveBit> സ്ഥിരം &bits() സ്ഥിരം { unpack(); തിരികെ bits_; }

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ width_; }

	ശൂന്യം append(DriveBit സ്ഥിരം &bit);

	ശൂന്യം append(DriveChunk സ്ഥിരം &chunk);

	ശൂന്യം pack() സ്ഥിരം;

	ശൂന്യം unpack() സ്ഥിരം;

	DriveBit &operator[](പൂർണ്ണസംഖ്യ index)
	{
		log_assert(index >= 0 && index < size());
		unpack();
		തിരികെ bits_[index];
	}

	സ്ഥിരം DriveBit &operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം
	{
		log_assert(index >= 0 && index < size());
		unpack();
		തിരികെ bits_[index];
	}

	ശൂന്യം clear()
	{
		chunks_.clear();
		bits_.clear();
		width_ = 0;
	}

	DriveSpec &operator=(DriveChunk സ്ഥിരം &chunk)
	{
		chunks_.clear();
		bits_.clear();
		append(chunk);
		തിരികെ *this;
	}

	DriveSpec &operator=(DriveChunkWire സ്ഥിരം &chunk) { തിരികെ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkPort സ്ഥിരം &chunk) { തിരികെ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMarker സ്ഥിരം &chunk) { തിരികെ *this = DriveChunk(chunk); }
	DriveSpec &operator=(DriveChunkMultiple സ്ഥിരം &chunk) { തിരികെ *this = DriveChunk(chunk); }

	DriveSpec &operator=(DriveBit സ്ഥിരം &bit)
	{
		chunks_.clear();
		bits_.clear();
		append(bit);
		തിരികെ *this;
	}

	DriveSpec &operator=(DriveBitWire സ്ഥിരം &bit) { തിരികെ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitPort സ്ഥിരം &bit) { തിരികെ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMarker സ്ഥിരം &bit) { തിരികെ *this = DriveBit(bit); }
	DriveSpec &operator=(DriveBitMultiple സ്ഥിരം &bit) { തിരികെ *this = DriveBit(bit); }

	ശൂന്യം updhash() സ്ഥിരം {
		എങ്കിൽ (hash_ != 0)
			തിരികെ;
		pack();
		hash_ = run_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;

	bool operator==(DriveSpec സ്ഥിരം &other) സ്ഥിരം {
		updhash();
		other.updhash();
		എങ്കിൽ (size() != other.size() || hash_ != other.hash_)
			തിരികെ false;
		തിരികെ chunks() == other.chunks();
	}

സ്വകാര്യ:
	ശൂന്യം compute_width();
};



ഘടന DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.setup(); }
	DriverMap(Design *design) { celltypes.setup(design); }

സ്വകാര്യ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ഘടന DriveBitId
	{
		പൂർണ്ണസംഖ്യ id = -1;

		DriveBitId() {};

		DriveBitId(പൂർണ്ണസംഖ്യ id) : id(id) { }

		bool operator==(സ്ഥിരം DriveBitId &other) സ്ഥിരം { തിരികെ id == other.id; }
		bool operator!=(സ്ഥിരം DriveBitId &other) സ്ഥിരം { തിരികെ id != other.id; }
		bool operator<(സ്ഥിരം DriveBitId &other) സ്ഥിരം { തിരികെ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ഘടന DriveBitGraph {
		dict<DriveBitId, DriveBitId> first_edges;
		dict<DriveBitId, DriveBitId> second_edges;
		dict<DriveBitId, pool<DriveBitId>> more_edges;

		ശൂന്യം add_edge(DriveBitId src, DriveBitId dst);
		DriveBitId എടുക്കുക_edge(DriveBitId src);
		ശൂന്യം clear(DriveBitId src);
		bool contains(DriveBitId src);
		പൂർണ്ണസംഖ്യ എണ്ണം(DriveBitId src);

		DriveBitId at(DriveBitId src, പൂർണ്ണസംഖ്യ index);
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
	പൂർണ്ണസംഖ്യ next_offset = 1 + (പൂർണ്ണസംഖ്യ)State::Sm;

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


	എണ്ണൽ class BitMode {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	BitMode bit_mode(DriveBit സ്ഥിരം &bit);
	DriveBitId id_from_drive_bit(DriveBit സ്ഥിരം &bit);
	DriveBit drive_bit_from_id(DriveBitId id);

	ശൂന്യം connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id);
	ശൂന്യം connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id);
	ശൂന്യം connect_undirected(DriveBitId a_id, DriveBitId b_id);

പൊതു:

	ശൂന്യം add(Module *module);

	// Add a single bit connection to the driver map.
	ശൂന്യം add(DriveBit സ്ഥിരം &a, DriveBit സ്ഥിരം &b);

	template<typename T>
	സ്ഥിതം constexpr bool is_sig_type() {
		തിരികെ
			std::is_same<T, SigSpec>::മൂല്യം ||
			std::is_same<T, SigChunk>::മൂല്യം ||
			std::is_same<T, DriveSpec>::മൂല്യം ||
			std::is_same<T, DriveChunk>::മൂല്യം ||
			std::is_same<T, DriveChunkPort>::മൂല്യം ||
			std::is_same<T, DriveChunkWire>::മൂല്യം ||
			std::is_same<T, Const>::മൂല്യം;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	template<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	add(T സ്ഥിരം &a, U സ്ഥിരം &b)
	{
		log_assert(a.size() == b.size());
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != GetSize(a); ++i)
			add(DriveBit(a[i]), DriveBit(b[i]));
	}


	// Specialized version that avoids unpacking
	ശൂന്യം add(SigSpec സ്ഥിരം &a, SigSpec സ്ഥിരം &b);

സ്വകാര്യ:
	ശൂന്യം add_port(Cell *cell, IdString port, SigSpec സ്ഥിരം &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<DriveBitId> orient_undirected_seen;
	pool<DriveBitId> orient_undirected_drivers;
	dict<DriveBitId, പൂർണ്ണസംഖ്യ> orient_undirected_distance;

	ശൂന്യം orient_undirected(DriveBitId id);

പൊതു:
	DriveBit operator()(DriveBit സ്ഥിരം &bit);

	DriveSpec operator()(DriveSpec spec);

സ്വകാര്യ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		തിരികെ wire->has_attribute(ID(keep));
	}
};

വരിയിൽ Hasher DriveBitWire::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(wire->name);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveBitPort::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveBitMarker::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(marker);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveBitMultiple::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(multiple_);
	തിരികെ h;
}

വരിയിൽ Hasher DriveBit::hash_into(Hasher h) സ്ഥിരം
{
	തിരഞ്ഞെടുപ്പ് (type_) {
	അവസ്ഥ DriveType::NONE:
		h.eat(0);
		നിർത്തുക;
	അവസ്ഥ DriveType::CONSTANT:
		h.eat(constant_);
		നിർത്തുക;
	അവസ്ഥ DriveType::WIRE:
		h.eat(wire_);
		നിർത്തുക;
	അവസ്ഥ DriveType::PORT:
		h.eat(port_);
		നിർത്തുക;
	അവസ്ഥ DriveType::MARKER:
		h.eat(marker_);
		നിർത്തുക;
	അവസ്ഥ DriveType::MULTIPLE:
		h.eat(multiple_);
		നിർത്തുക;
	}
	h.eat(type_);
	തിരികെ h;
}

വരിയിൽ Hasher DriveChunkWire::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(wire->name);
	h.eat(width);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveChunkPort::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(cell->name);
	h.eat(port);
	h.eat(width);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveChunkMarker::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(marker);
	h.eat(width);
	h.eat(offset);
	തിരികെ h;
}

വരിയിൽ Hasher DriveChunkMultiple::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(width_);
	h.eat(multiple_);
	തിരികെ h;
}

വരിയിൽ Hasher DriveChunk::hash_into(Hasher h) സ്ഥിരം
{
	തിരഞ്ഞെടുപ്പ് (type_) {
	അവസ്ഥ DriveType::NONE:
		h.eat(0);
		നിർത്തുക;
	അവസ്ഥ DriveType::CONSTANT:
		h.eat(constant_);
		നിർത്തുക;
	അവസ്ഥ DriveType::WIRE:
		h.eat(wire_);
		നിർത്തുക;
	അവസ്ഥ DriveType::PORT:
		h.eat(port_);
		നിർത്തുക;
	അവസ്ഥ DriveType::MARKER:
		h.eat(marker_);
		നിർത്തുക;
	അവസ്ഥ DriveType::MULTIPLE:
		h.eat(multiple_);
		നിർത്തുക;
	}
	h.eat(type_);
	തിരികെ h;
}

വരിയിൽ Hasher DriveSpec::hash_into(Hasher h) സ്ഥിരം
{
	updhash();
	h.eat(hash_);
	തിരികെ h;
}

വരിയിൽ Hasher DriverMap::DriveBitId::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(id);
	തിരികെ h;
}

YOSYS_NAMESPACE_END

#endif
