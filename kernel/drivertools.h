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

#اگر_غیر_تعریف_شدہ DRIVERTOOLS_H
#تعریف_کرو DRIVERTOOLS_H

#شامل <string>
#شامل <type_traits>

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ساخت Drive_ثنائی_ہندسہ;

ساخت DriveChunkWire;
ساخت Drive_Chunk_پورٹ;
ساخت DriveChunkMarker;
ساخت DriveChunk;

ساخت DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(Drive_Chunk_پورٹ ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

شمار class DriveType : غیرعلامتی حرف
{
	NONE,
	CONSTANT,
	WIRE,
	پورٹ_2,
	مضرب,
	MARKER,
};

ساخت Drive_ثنائی_ہندسہ_Wire
{
	Wire *wire;
	صحیح انحراف;

	Drive_ثنائی_ہندسہ_Wire(Wire *wire, صحیح انحراف) : wire(wire), انحراف(انحراف) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_Wire &other) ثابت
	{
		واپس wire == other.wire && انحراف == other.انحراف;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_Wire &other) ثابت
	{
		اگر (wire != other.wire)
			واپس wire->name < other.wire->name;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	عامل Sig_ثنائی_ہندسہ() ثابت
	{
		واپس Sig_ثنائی_ہندسہ(wire, انحراف);
	}
};

ساخت Drive_ثنائی_ہندسہ_پورٹ
{
	Cell *cell;
	IdString پورٹ;
	صحیح انحراف;

	Drive_ثنائی_ہندسہ_پورٹ(Cell *cell, IdString پورٹ, صحیح انحراف) : cell(cell), پورٹ(پورٹ), انحراف(انحراف) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_پورٹ &other) ثابت
	{
		واپس cell == other.cell && پورٹ == other.پورٹ && انحراف == other.انحراف;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_پورٹ &other) ثابت
	{
		اگر (cell != other.cell)
			واپس cell->name < other.cell->name;
		اگر (پورٹ != other.پورٹ)
			واپس پورٹ < other.پورٹ;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


ساخت Drive_ثنائی_ہندسہ_Marker
{
	صحیح marker;
	صحیح انحراف;

	Drive_ثنائی_ہندسہ_Marker(صحیح marker, صحیح انحراف) : marker(marker), انحراف(انحراف) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_Marker &other) ثابت
	{
		واپس marker == other.marker && انحراف == other.انحراف;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_Marker &other) ثابت
	{
		اگر (marker != other.marker)
			واپس marker < other.marker;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

ساخت Drive_ثنائی_ہندسہ_مضرب
{
نجی:
	pool<Drive_ثنائی_ہندسہ> مضرب_3;

عوامی:
	Drive_ثنائی_ہندسہ_مضرب();
	Drive_ثنائی_ہندسہ_مضرب(Drive_ثنائی_ہندسہ ثابت &single);

	pool<Drive_ثنائی_ہندسہ> ثابت &مضرب_2() ثابت { واپس مضرب_3; }

	خالی ملاؤ(Drive_ثنائی_ہندسہ_مضرب ثابت &other)
	{
		برائے (Drive_ثنائی_ہندسہ ثابت &single : other.مضرب_3)
			ملاؤ(single);
	}

	خالی ملاؤ(Drive_ثنائی_ہندسہ_مضرب &&other)
	{
		برائے (Drive_ثنائی_ہندسہ &single : other.مضرب_3)
			ملاؤ(std::move(single));
	}

	خالی ملاؤ(Drive_ثنائی_ہندسہ ثابت &single);
	خالی ملاؤ(Drive_ثنائی_ہندسہ &&single);

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_مضرب &other) ثابت
	{
		واپس مضرب_3 == other.مضرب_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساخت Drive_ثنائی_ہندسہ
{
نجی:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		Drive_ثنائی_ہندسہ_Wire wire_;
		Drive_ثنائی_ہندسہ_پورٹ پورٹ_3;
		Drive_ثنائی_ہندسہ_Marker marker_;
		Drive_ثنائی_ہندسہ_مضرب مضرب_3;
	};
عوامی:
	Drive_ثنائی_ہندسہ()  {}

	Drive_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);

	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ ثابت &other) { *this = other; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ &&other) { *this = other; }


	Drive_ثنائی_ہندسہ(State constant) { *this = constant; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_Wire ثابت &wire) { *this = wire; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_Wire &&wire) { *this = wire; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_پورٹ ثابت &پورٹ) { *this = پورٹ; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_پورٹ &&پورٹ) { *this = پورٹ; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_Marker ثابت &marker) { *this = marker; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_Marker &&marker) { *this = marker; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	Drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ_مضرب &&مضرب_2) { *this = مضرب_2; }

	~Drive_ثنائی_ہندسہ() { سیٹ_none(); }

	خالی سیٹ_none()
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				توڑو;
			حالت DriveType::CONSTANT:
				توڑو;
			حالت DriveType::WIRE:
				wire_.~Drive_ثنائی_ہندسہ_Wire();
				توڑو;
			حالت DriveType::پورٹ_2:
				پورٹ_3.~Drive_ثنائی_ہندسہ_پورٹ();
				توڑو;
			حالت DriveType::MARKER:
				marker_.~Drive_ثنائی_ہندسہ_Marker();
				توڑو;
			حالت DriveType::مضرب:
				مضرب_3.~Drive_ثنائی_ہندسہ_مضرب();
				توڑو;
		}
		type_ = DriveType::NONE;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ ثابت &other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				سیٹ_none();
				توڑو;
			حالت DriveType::CONSTANT:
				*this = other.constant_;
				توڑو;
			حالت DriveType::WIRE:
				*this = other.wire_;
				توڑو;
			حالت DriveType::پورٹ_2:
				*this = other.پورٹ_3;
				توڑو;
			حالت DriveType::MARKER:
				*this = other.marker_;
				توڑو;
			حالت DriveType::مضرب:
				*this = other.مضرب_3;
				توڑو;
		}
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ &&other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				سیٹ_none();
				توڑو;
			حالت DriveType::CONSTANT:
				*this = std::move(other.constant_);
				توڑو;
			حالت DriveType::WIRE:
				*this = std::move(other.wire_);
				توڑو;
			حالت DriveType::پورٹ_2:
				*this = std::move(other.پورٹ_3);
				توڑو;
			حالت DriveType::MARKER:
				*this = std::move(other.marker_);
				توڑو;
			حالت DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				توڑو;
		}
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(State constant)
	{
		سیٹ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Wire ثابت &wire)
	{
		سیٹ_none();
		new (&wire_) Drive_ثنائی_ہندسہ_Wire(wire);
		type_ = DriveType::WIRE;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Wire &&wire)
	{
		سیٹ_none();
		new (&wire_) Drive_ثنائی_ہندسہ_Wire(wire);
		type_ = DriveType::WIRE;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_پورٹ ثابت &پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_ثنائی_ہندسہ_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_پورٹ &&پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_ثنائی_ہندسہ_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Marker ثابت &marker)
	{
		سیٹ_none();
		new (&marker_) Drive_ثنائی_ہندسہ_Marker(marker);
		type_ = DriveType::MARKER;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Marker &&marker)
	{
		سیٹ_none();
		new (&marker_) Drive_ثنائی_ہندسہ_Marker(marker);
		type_ = DriveType::MARKER;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_مضرب ثابت &مضرب_2)
	{
		سیٹ_none();
		اگر (مضرب_2.مضرب_2().خالی_خانے())
			واپس *this;
		new (&مضرب_3) Drive_ثنائی_ہندسہ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		واپس *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_مضرب &&مضرب_2)
	{
		سیٹ_none();
		اگر (مضرب_2.مضرب_2().خالی_خانے())
			واپس *this;
		new (&مضرب_3) Drive_ثنائی_ہندسہ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		واپس *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		اگر (type_ != other.type_)
			واپس false;

		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس true;
			حالت DriveType::CONSTANT:
				واپس constant_ == other.constant_;
			حالت DriveType::WIRE:
				واپس wire_ == other.wire_;
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3 == other.پورٹ_3;
			حالت DriveType::MARKER:
				واپس marker_ == other.marker_;
			حالت DriveType::مضرب:
				واپس مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		واپس !(*this == other);
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		اگر (type_ != other.type_)
			واپس type_ < other.type_;
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس false;
			حالت DriveType::CONSTANT:
				واپس constant_ < other.constant_;
			حالت DriveType::WIRE:
				واپس wire_ < other.wire_;
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3 < other.پورٹ_3;
			حالت DriveType::MARKER:
				واپس marker_ < other.marker_;
			حالت DriveType::مضرب:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { واپس type_; }

	bool is_none() ثابت { واپس type_ == DriveType::NONE; }
	bool is_constant() ثابت { واپس type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { واپس type_ == DriveType::WIRE; }
	bool is_پورٹ() ثابت { واپس type_ == DriveType::پورٹ_2; }
	bool is_marker() ثابت { واپس type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { واپس type_ == DriveType::مضرب; }

	State &constant() { log_assert(is_constant()); واپس constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); واپس constant_; }
	Drive_ثنائی_ہندسہ_Wire &wire() { log_assert(is_wire()); واپس wire_; }
	Drive_ثنائی_ہندسہ_Wire ثابت &wire() ثابت { log_assert(is_wire()); واپس wire_; }
	Drive_ثنائی_ہندسہ_پورٹ &پورٹ() { log_assert(is_پورٹ()); واپس پورٹ_3; }
	Drive_ثنائی_ہندسہ_پورٹ ثابت &پورٹ() ثابت { log_assert(is_پورٹ()); واپس پورٹ_3; }
	Drive_ثنائی_ہندسہ_Marker &marker() { log_assert(is_marker()); واپس marker_; }
	Drive_ثنائی_ہندسہ_Marker ثابت &marker() ثابت { log_assert(is_marker()); واپس marker_; }
	Drive_ثنائی_ہندسہ_مضرب &مضرب_2() { log_assert(is_مضرب()); واپس مضرب_3; }
	Drive_ثنائی_ہندسہ_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); واپس مضرب_3; }

	خالی ملاؤ(Drive_ثنائی_ہندسہ ثابت &other);

};

اندرونی Drive_ثنائی_ہندسہ_مضرب::Drive_ثنائی_ہندسہ_مضرب() {}
اندرونی Drive_ثنائی_ہندسہ_مضرب::Drive_ثنائی_ہندسہ_مضرب(Drive_ثنائی_ہندسہ ثابت &single)
{
	مضرب_3.emplace(single);
}

ساخت DriveChunkWire
{
	Wire *wire;
	صحیح انحراف;
	صحیح چوڑائی;

	DriveChunkWire(Wire *wire, صحیح انحراف, صحیح چوڑائی) : wire(wire), انحراف(انحراف), چوڑائی(چوڑائی) {}
	DriveChunkWire(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) : wire(ثنائی_ہندسہ.wire), انحراف(ثنائی_ہندسہ.انحراف), چوڑائی(1) {}

	صحیح سائز() ثابت { واپس چوڑائی; }

	Drive_ثنائی_ہندسہ_Wire عامل[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		واپس Drive_ثنائی_ہندسہ_Wire(wire, انحراف + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_پورا() ثابت { واپس انحراف == 0 && چوڑائی == wire->چوڑائی; }

	bool عامل==(ثابت DriveChunkWire &other) ثابت
	{
		واپس wire == other.wire && انحراف == other.انحراف && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت DriveChunkWire &other) ثابت
	{
		اگر (wire != other.wire)
			واپس wire->name < other.wire->name;
		اگر (چوڑائی != other.چوڑائی)
			واپس چوڑائی < other.چوڑائی;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit عامل SigChunk() ثابت
	{
		واپس SigChunk(wire, انحراف, چوڑائی);
	}
};

ساخت Drive_Chunk_پورٹ
{
	Cell *cell;
	IdString پورٹ;
	صحیح انحراف;
	صحیح چوڑائی;

	Drive_Chunk_پورٹ(Cell *cell, IdString پورٹ, صحیح انحراف, صحیح چوڑائی) :
		cell(cell), پورٹ(پورٹ), انحراف(انحراف), چوڑائی(چوڑائی) { }
	Drive_Chunk_پورٹ(Cell *cell, IdString پورٹ) :
		cell(cell), پورٹ(پورٹ), انحراف(0), چوڑائی(Get_سائز(cell->connections().at(پورٹ))) { }
	Drive_Chunk_پورٹ(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), پورٹ(conn.پہلا), انحراف(0), چوڑائی(Get_سائز(conn.دوسرا)) { }
	Drive_Chunk_پورٹ(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) :
		cell(ثنائی_ہندسہ.cell), پورٹ(ثنائی_ہندسہ.پورٹ), انحراف(ثنائی_ہندسہ.انحراف), چوڑائی(1) { }

	صحیح سائز() ثابت { واپس چوڑائی; }

	Drive_ثنائی_ہندسہ_پورٹ عامل[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		واپس Drive_ثنائی_ہندسہ_پورٹ(cell, پورٹ, انحراف + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ);
	bool try_append(Drive_Chunk_پورٹ ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_پورا() ثابت
	{
		واپس انحراف == 0 && چوڑائی == cell->connections().at(پورٹ).سائز();
	}

	bool عامل==(ثابت Drive_Chunk_پورٹ &other) ثابت
	{
		واپس cell == other.cell && پورٹ == other.پورٹ && انحراف == other.انحراف && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت Drive_Chunk_پورٹ &other) ثابت
	{
		اگر (cell != other.cell)
			واپس cell->name < other.cell->name;
		اگر (پورٹ != other.پورٹ)
			واپس پورٹ < other.پورٹ;
		اگر (چوڑائی != other.چوڑائی)
			واپس چوڑائی < other.چوڑائی;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


ساخت DriveChunkMarker
{
	صحیح marker;
	صحیح انحراف;
	صحیح چوڑائی;

	DriveChunkMarker(صحیح marker, صحیح انحراف, صحیح چوڑائی) :
		marker(marker), انحراف(انحراف), چوڑائی(چوڑائی) {}
	DriveChunkMarker(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) :
		marker(ثنائی_ہندسہ.marker), انحراف(ثنائی_ہندسہ.انحراف), چوڑائی(1) {}

	صحیح سائز() ثابت { واپس چوڑائی; }

	Drive_ثنائی_ہندسہ_Marker عامل[](صحیح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		واپس Drive_ثنائی_ہندسہ_Marker(marker, انحراف + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool عامل==(ثابت DriveChunkMarker &other) ثابت
	{
		واپس marker == other.marker && انحراف == other.انحراف && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت DriveChunkMarker &other) ثابت
	{
		اگر (marker != other.marker)
			واپس marker < other.marker;
		اگر (چوڑائی != other.چوڑائی)
			واپس چوڑائی < other.چوڑائی;
		واپس انحراف < other.انحراف;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساخت Drive_Chunk_مضرب
{
نجی:
	mutable pool<DriveChunk> مضرب_3;
	صحیح چوڑائی_3;

عوامی:
	pool<DriveChunk> ثابت &مضرب_2() ثابت { واپس مضرب_3; }

	Drive_Chunk_مضرب(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ);

	صحیح سائز() ثابت { واپس چوڑائی_3; }

	Drive_ثنائی_ہندسہ_مضرب عامل[](صحیح i) ثابت;

	bool can_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) ثابت;

	bool try_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ);


	bool can_append(Drive_Chunk_مضرب ثابت &ثنائی_ہندسہ) ثابت;

	bool try_append(Drive_Chunk_مضرب ثابت &ثنائی_ہندسہ);

	bool عامل==(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		واپس چوڑائی_3 == other.چوڑائی_3 && مضرب_3 == other.مضرب_3;
	}

	bool عامل<(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		اگر (مضرب_3.سائز() < other.مضرب_3.سائز())

		مضرب_3.ترتیب_دو();
		واپس false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساخت DriveChunk
{
نجی:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		صحیح none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_پورٹ پورٹ_3;
		DriveChunkMarker marker_;
		Drive_Chunk_مضرب مضرب_3;
	};

عوامی:
	DriveChunk() { سیٹ_none(); }

	DriveChunk(DriveChunk ثابت &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_ثنائی_ہندسہ ثابت &other) { *this = other; }

	DriveChunk(Const ثابت &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ثابت &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_پورٹ ثابت &پورٹ) { *this = پورٹ; }
	DriveChunk(Drive_Chunk_پورٹ &&پورٹ) { *this = پورٹ; }
	DriveChunk(DriveChunkMarker ثابت &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_مضرب ثابت &مضرب_2) { *this = مضرب_2; }
	DriveChunk(Drive_Chunk_مضرب &&مضرب_2) { *this = مضرب_2; }

	~DriveChunk() { سیٹ_none(); }

	Drive_ثنائی_ہندسہ عامل[](صحیح i) ثابت
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس Drive_ثنائی_ہندسہ();
			حالت DriveType::CONSTANT:
				واپس constant_[i];
			حالت DriveType::WIRE:
				واپس wire_[i];
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3[i];
			حالت DriveType::MARKER:
				واپس marker_[i];
			حالت DriveType::مضرب:
				واپس مضرب_3[i];
		}
		log_abort();
	}

	خالی سیٹ_none(صحیح چوڑائی = 0)
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				none_ = چوڑائی;
				توڑو;
			حالت DriveType::CONSTANT:
				constant_.~Const();
				توڑو;
			حالت DriveType::WIRE:
				wire_.~DriveChunkWire();
				توڑو;
			حالت DriveType::پورٹ_2:
				پورٹ_3.~Drive_Chunk_پورٹ();
				توڑو;
			حالت DriveType::MARKER:
				marker_.~DriveChunkMarker();
				توڑو;
			حالت DriveType::مضرب:
				مضرب_3.~Drive_Chunk_مضرب();
				توڑو;
		}
		type_ = DriveType::NONE;
		none_ = چوڑائی;
	}

	DriveChunk &عامل=(DriveChunk ثابت &other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				سیٹ_none(other.none_);
				توڑو;
			حالت DriveType::CONSTANT:
				*this = other.constant_;
				توڑو;
			حالت DriveType::WIRE:
				*this = other.wire_;
				توڑو;
			حالت DriveType::پورٹ_2:
				*this = other.پورٹ_3;
				توڑو;
			حالت DriveType::MARKER:
				*this = other.marker_;
				توڑو;
			حالت DriveType::مضرب:
				*this = other.مضرب_3;
				توڑو;
		}
		واپس *this;
	}

	DriveChunk &عامل=(DriveChunk &&other)
	{
		انتخاب (other.type_)
		{
			حالت DriveType::NONE:
				سیٹ_none(other.none_);
				توڑو;
			حالت DriveType::CONSTANT:
				*this = std::move(other.constant_);
				توڑو;
			حالت DriveType::WIRE:
				*this = std::move(other.wire_);
				توڑو;
			حالت DriveType::پورٹ_2:
				*this = std::move(other.پورٹ_3);
				توڑو;
			حالت DriveType::MARKER:
				*this = std::move(other.marker_);
				توڑو;
			حالت DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				توڑو;
		}
		واپس *this;
	}

	DriveChunk &عامل=(Const ثابت &constant)
	{
		سیٹ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		واپس *this;
	}

	DriveChunk &عامل=(Const &&constant)
	{
		سیٹ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		واپس *this;
	}

	DriveChunk &عامل=(DriveChunkWire ثابت &wire)
	{
		سیٹ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		واپس *this;
	}

	DriveChunk &عامل=(DriveChunkWire &&wire)
	{
		سیٹ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		واپس *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٹ ثابت &پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_Chunk_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		واپس *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٹ &&پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_Chunk_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		واپس *this;
	}

	DriveChunk &عامل=(DriveChunkMarker ثابت &marker)
	{
		سیٹ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		واپس *this;
	}

	DriveChunk &عامل=(DriveChunkMarker &&marker)
	{
		سیٹ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		واپس *this;
	}

	DriveChunk &عامل=(Drive_Chunk_مضرب ثابت &مضرب_2)
	{
		سیٹ_none(مضرب_2.سائز());
		اگر (مضرب_2.مضرب_2().خالی_خانے())
			واپس *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		واپس *this;
	}

	DriveChunk &عامل=(Drive_Chunk_مضرب &&مضرب_2)
	{
		سیٹ_none(مضرب_2.سائز());
		اگر (مضرب_2.مضرب_2().خالی_خانے())
			واپس *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		واپس *this;
	}

	DriveChunk &عامل=(Drive_ثنائی_ہندسہ ثابت &other)
	{
		انتخاب (other.type())
		{
			حالت DriveType::NONE:
				سیٹ_none(1);
				توڑو;
			حالت DriveType::CONSTANT:
				*this = Const(other.constant());
				توڑو;
			حالت DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				توڑو;
			حالت DriveType::پورٹ_2:
				*this = Drive_Chunk_پورٹ(other.پورٹ());
				توڑو;
			حالت DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				توڑو;
			حالت DriveType::مضرب:
				*this = Drive_Chunk_مضرب(other.مضرب_2());
				توڑو;
		}
		واپس *this;
	}

	bool can_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت DriveChunk &other) ثابت
	{
		اگر (type_ != other.type_)
			واپس false;

		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس true;
			حالت DriveType::CONSTANT:
				واپس constant_ == other.constant_;
			حالت DriveType::WIRE:
				واپس wire_ == other.wire_;
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3 == other.پورٹ_3;
			حالت DriveType::MARKER:
				واپس marker_ == other.marker_;
			حالت DriveType::مضرب:
				واپس مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت DriveChunk &other) ثابت
	{
		واپس !(*this == other);
	}

	bool عامل<(ثابت DriveChunk &other) ثابت
	{
		اگر (type_ != other.type_)
			واپس type_ < other.type_;
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس false;
			حالت DriveType::CONSTANT:
				واپس constant_ < other.constant_;
			حالت DriveType::WIRE:
				واپس wire_ < other.wire_;
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3 < other.پورٹ_3;
			حالت DriveType::MARKER:
				واپس marker_ < other.marker_;
			حالت DriveType::مضرب:
				واپس مضرب_3 < other.مضرب_3;
		}
		log_abort();
	}

	DriveType type() ثابت { واپس type_; }

	bool is_none() ثابت { واپس type_ == DriveType::NONE; }
	bool is_constant() ثابت { واپس type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { واپس type_ == DriveType::WIRE; }
	bool is_پورٹ() ثابت { واپس type_ == DriveType::پورٹ_2; }
	bool is_marker() ثابت { واپس type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { واپس type_ == DriveType::مضرب; }

	Const &constant() { log_assert(is_constant()); واپس constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); واپس constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); واپس wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); واپس wire_; }
	Drive_Chunk_پورٹ &پورٹ() { log_assert(is_پورٹ()); واپس پورٹ_3; }
	Drive_Chunk_پورٹ ثابت &پورٹ() ثابت { log_assert(is_پورٹ()); واپس پورٹ_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); واپس marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); واپس marker_; }
	Drive_Chunk_مضرب &مضرب_2() { log_assert(is_مضرب()); واپس مضرب_3; }
	Drive_Chunk_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); واپس مضرب_3; }


	صحیح سائز() ثابت
	{
		انتخاب (type_)
		{
			حالت DriveType::NONE:
				واپس none_;
			حالت DriveType::CONSTANT:
				واپس constant_.سائز();
			حالت DriveType::WIRE:
				واپس wire_.سائز();
			حالت DriveType::پورٹ_2:
				واپس پورٹ_3.سائز();
			حالت DriveType::MARKER:
				واپس marker_.سائز();
			حالت DriveType::مضرب:
				واپس مضرب_3.سائز();
		}
		log_abort();
	}
};

اندرونی Drive_Chunk_مضرب::Drive_Chunk_مضرب(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ)
	: چوڑائی_3(1)
{
	برائے (خودکار ثابت &ثنائی_ہندسہ : ثنائی_ہندسہ.مضرب_2())
		مضرب_3.emplace(ثنائی_ہندسہ);
}

ساخت DriveSpec
{
نجی:
	صحیح چوڑائی_3 = 0;
	mutable std::سمتیہ<DriveChunk> chunks_;
	mutable std::سمتیہ<Drive_ثنائی_ہندسہ> بٹ_2;
	mutable غیرعلامتی صحیح hash_ = 0;
عوامی:

	اندرونی bool packed() ثابت {
		واپس بٹ_2.خالی_خانے();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_پورٹ ثابت &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ثابت &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_مضرب ثابت &chunk) { *this = chunk; }

	DriveSpec(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) { *this = ثنائی_ہندسہ; }
	DriveSpec(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) { *this = ثنائی_ہندسہ; }
	DriveSpec(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) { *this = ثنائی_ہندسہ; }
	DriveSpec(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) { *this = ثنائی_ہندسہ; }
	DriveSpec(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) { *this = ثنائی_ہندسہ; }

	DriveSpec(std::سمتیہ<DriveChunk> ثابت &chunks) : chunks_(chunks) { compute_چوڑائی(); }

	DriveSpec(std::سمتیہ<Drive_ثنائی_ہندسہ> ثابت &بٹ)
	{
		برائے (خودکار ثابت &ثنائی_ہندسہ : بٹ)
			append(ثنائی_ہندسہ);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		برائے (خودکار ثابت &ثنائی_ہندسہ : sig.بٹ())
			append(ثنائی_ہندسہ);
	}

	std::سمتیہ<DriveChunk> ثابت &chunks() ثابت { pack(); واپس chunks_; }
	std::سمتیہ<Drive_ثنائی_ہندسہ> ثابت &بٹ() ثابت { unpack(); واپس بٹ_2; }

	صحیح سائز() ثابت { واپس چوڑائی_3; }

	خالی append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);

	خالی append(DriveChunk ثابت &chunk);

	خالی pack() ثابت;

	خالی unpack() ثابت;

	Drive_ثنائی_ہندسہ &عامل[](صحیح اشاریہ)
	{
		log_assert(اشاریہ >= 0 && اشاریہ < سائز());
		unpack();
		واپس بٹ_2[اشاریہ];
	}

	ثابت Drive_ثنائی_ہندسہ &عامل[](صحیح اشاریہ) ثابت
	{
		log_assert(اشاریہ >= 0 && اشاریہ < سائز());
		unpack();
		واپس بٹ_2[اشاریہ];
	}

	خالی clear()
	{
		chunks_.clear();
		بٹ_2.clear();
		چوڑائی_3 = 0;
	}

	DriveSpec &عامل=(DriveChunk ثابت &chunk)
	{
		chunks_.clear();
		بٹ_2.clear();
		append(chunk);
		واپس *this;
	}

	DriveSpec &عامل=(DriveChunkWire ثابت &chunk) { واپس *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_پورٹ ثابت &chunk) { واپس *this = DriveChunk(chunk); }
	DriveSpec &عامل=(DriveChunkMarker ثابت &chunk) { واپس *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_مضرب ثابت &chunk) { واپس *this = DriveChunk(chunk); }

	DriveSpec &عامل=(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
	{
		chunks_.clear();
		بٹ_2.clear();
		append(ثنائی_ہندسہ);
		واپس *this;
	}

	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) { واپس *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) { واپس *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) { واپس *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) { واپس *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }

	خالی updhash() ثابت {
		اگر (hash_ != 0)
			واپس;
		pack();
		hash_ = تسلسل_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		اگر (سائز() != other.سائز() || hash_ != other.hash_)
			واپس false;
		واپس chunks() == other.chunks();
	}

نجی:
	خالی compute_چوڑائی();
};



ساخت DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.تیاری(); }
	DriverMap(Design *design) { celltypes.تیاری(design); }

نجی:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ساخت Drive_ثنائی_ہندسہ_Id
	{
		صحیح id = -1;

		Drive_ثنائی_ہندسہ_Id() {};

		Drive_ثنائی_ہندسہ_Id(صحیح id) : id(id) { }

		bool عامل==(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { واپس id == other.id; }
		bool عامل!=(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { واپس id != other.id; }
		bool عامل<(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { واپس id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ساخت Drive_ثنائی_ہندسہ_گراف {
		dict<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ_Id> پہلا_کنارے;
		dict<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ_Id> دوسرا_کنارے;
		dict<Drive_ثنائی_ہندسہ_Id, pool<Drive_ثنائی_ہندسہ_Id>> مزید_کنارے;

		خالی جوڑو_کنارہ(Drive_ثنائی_ہندسہ_Id src, Drive_ثنائی_ہندسہ_Id dst);
		Drive_ثنائی_ہندسہ_Id نکالو_کنارہ(Drive_ثنائی_ہندسہ_Id src);
		خالی clear(Drive_ثنائی_ہندسہ_Id src);
		bool شامل_ہے(Drive_ثنائی_ہندسہ_Id src);
		صحیح گنتی(Drive_ثنائی_ہندسہ_Id src);

		Drive_ثنائی_ہندسہ_Id at(Drive_ثنائی_ہندسہ_Id src, صحیح اشاریہ);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_ثنائی_ہندسہ_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_ثنائی_ہندسہ_Id> پورٹ_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ> drive_بٹ;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ> isolated_drive_بٹ;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	صحیح اگلا_انحراف = 1 + (صحیح)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_ثنائی_ہندسہ_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_ثنائی_ہندسہ_گراف connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_ثنائی_ہندسہ_گراف connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_ثنائی_ہندسہ_گراف connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_ثنائی_ہندسہ_Id> oriented_present;


	شمار class ثنائی_ہندسہ_منوال {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	ثنائی_ہندسہ_منوال ثنائی_ہندسہ_منوال_2(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);
	Drive_ثنائی_ہندسہ_Id id_from_drive_ثنائی_ہندسہ(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);
	Drive_ثنائی_ہندسہ drive_ثنائی_ہندسہ_from_id(Drive_ثنائی_ہندسہ_Id id);

	خالی connect_directed_ملاؤ(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id);
	خالی connect_directed_بفر(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id);
	خالی connect_undirected(Drive_ثنائی_ہندسہ_Id a_id, Drive_ثنائی_ہندسہ_Id b_id);

عوامی:

	خالی جوڑو(Module *module);

	// Add a single bit connection to the driver map.
	خالی جوڑو(Drive_ثنائی_ہندسہ ثابت &a, Drive_ثنائی_ہندسہ ثابت &b);

	سانچہ<typename T>
	ساکن constexpr bool is_sig_type() {
		واپس
			std::is_same<T, SigSpec>::قدر ||
			std::is_same<T, SigChunk>::قدر ||
			std::is_same<T, DriveSpec>::قدر ||
			std::is_same<T, DriveChunk>::قدر ||
			std::is_same<T, Drive_Chunk_پورٹ>::قدر ||
			std::is_same<T, DriveChunkWire>::قدر ||
			std::is_same<T, Const>::قدر;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	سانچہ<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	جوڑو(T ثابت &a, U ثابت &b)
	{
		log_assert(a.سائز() == b.سائز());
		برائے (صحیح i = 0; i != Get_سائز(a); ++i)
			جوڑو(Drive_ثنائی_ہندسہ(a[i]), Drive_ثنائی_ہندسہ(b[i]));
	}


	// Specialized version that avoids unpacking
	خالی جوڑو(SigSpec ثابت &a, SigSpec ثابت &b);

نجی:
	خالی جوڑو_پورٹ(Cell *cell, IdString پورٹ, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_ثنائی_ہندسہ_Id> orient_undirected_نظر_آیا;
	pool<Drive_ثنائی_ہندسہ_Id> orient_undirected_drivers;
	dict<Drive_ثنائی_ہندسہ_Id, صحیح> orient_undirected_فاصلہ;

	خالی orient_undirected(Drive_ثنائی_ہندسہ_Id id);

عوامی:
	Drive_ثنائی_ہندسہ عامل()(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);

	DriveSpec عامل()(DriveSpec spec);

نجی:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		واپس wire->has_attribute(ID(keep));
	}
};

اندرونی Hasher Drive_ثنائی_ہندسہ_Wire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher Drive_ثنائی_ہندسہ_پورٹ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٹ);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher Drive_ثنائی_ہندسہ_Marker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher Drive_ثنائی_ہندسہ_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(مضرب_3);
	واپس h;
}

اندرونی Hasher Drive_ثنائی_ہندسہ::hash_into(Hasher h) ثابت
{
	انتخاب (type_) {
	حالت DriveType::NONE:
		h.eat(0);
		توڑو;
	حالت DriveType::CONSTANT:
		h.eat(constant_);
		توڑو;
	حالت DriveType::WIRE:
		h.eat(wire_);
		توڑو;
	حالت DriveType::پورٹ_2:
		h.eat(پورٹ_3);
		توڑو;
	حالت DriveType::MARKER:
		h.eat(marker_);
		توڑو;
	حالت DriveType::مضرب:
		h.eat(مضرب_3);
		توڑو;
	}
	h.eat(type_);
	واپس h;
}

اندرونی Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(چوڑائی);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher Drive_Chunk_پورٹ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٹ);
	h.eat(چوڑائی);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(چوڑائی);
	h.eat(انحراف);
	واپس h;
}

اندرونی Hasher Drive_Chunk_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(چوڑائی_3);
	h.eat(مضرب_3);
	واپس h;
}

اندرونی Hasher DriveChunk::hash_into(Hasher h) ثابت
{
	انتخاب (type_) {
	حالت DriveType::NONE:
		h.eat(0);
		توڑو;
	حالت DriveType::CONSTANT:
		h.eat(constant_);
		توڑو;
	حالت DriveType::WIRE:
		h.eat(wire_);
		توڑو;
	حالت DriveType::پورٹ_2:
		h.eat(پورٹ_3);
		توڑو;
	حالت DriveType::MARKER:
		h.eat(marker_);
		توڑو;
	حالت DriveType::مضرب:
		h.eat(مضرب_3);
		توڑو;
	}
	h.eat(type_);
	واپس h;
}

اندرونی Hasher DriveSpec::hash_into(Hasher h) ثابت
{
	updhash();
	h.eat(hash_);
	واپس h;
}

اندرونی Hasher DriverMap::Drive_ثنائی_ہندسہ_Id::hash_into(Hasher h) ثابت
{
	h.eat(id);
	واپس h;
}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
