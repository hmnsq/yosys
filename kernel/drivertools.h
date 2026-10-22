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

#جے_بنا_تعریف DRIVERTOOLS_H
#تعریف_کرو DRIVERTOOLS_H

#ضمن <string>
#ضمن <type_traits>

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

بنية Drive_ثنائی_ہندسہ;

بنية DriveChunkWire;
بنية Drive_Chunk_پورٹ;
بنية DriveChunkMarker;
بنية DriveChunk;

بنية DriveSpec;

std::string log_signal(DriveChunkWire ثابت &chunk);
std::string log_signal(Drive_Chunk_پورٹ ثابت &chunk);
std::string log_signal(DriveChunkMarker ثابت &chunk);
std::string log_signal(DriveChunk ثابت &chunk);
std::string log_signal(DriveSpec ثابت &chunk);

تعداد class DriveType : بنانشانی حرف
{
	NONE,
	CONSTANT,
	WIRE,
	پورٹ_2,
	مضرب,
	MARKER,
};

بنية Drive_ثنائی_ہندسہ_Wire
{
	Wire *wire;
	صحيح وتھ;

	Drive_ثنائی_ہندسہ_Wire(Wire *wire, صحيح وتھ) : wire(wire), وتھ(وتھ) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_Wire &other) ثابت
	{
		أرجع wire == other.wire && وتھ == other.وتھ;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_Wire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;


	عامل Sig_ثنائی_ہندسہ() ثابت
	{
		أرجع Sig_ثنائی_ہندسہ(wire, وتھ);
	}
};

بنية Drive_ثنائی_ہندسہ_پورٹ
{
	Cell *cell;
	IdString پورٹ;
	صحيح وتھ;

	Drive_ثنائی_ہندسہ_پورٹ(Cell *cell, IdString پورٹ, صحيح وتھ) : cell(cell), پورٹ(پورٹ), وتھ(وتھ) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_پورٹ &other) ثابت
	{
		أرجع cell == other.cell && پورٹ == other.پورٹ && وتھ == other.وتھ;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_پورٹ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورٹ != other.پورٹ)
			أرجع پورٹ < other.پورٹ;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};


بنية Drive_ثنائی_ہندسہ_Marker
{
	صحيح marker;
	صحيح وتھ;

	Drive_ثنائی_ہندسہ_Marker(صحيح marker, صحيح وتھ) : marker(marker), وتھ(وتھ) {}

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_Marker &other) ثابت
	{
		أرجع marker == other.marker && وتھ == other.وتھ;
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ_Marker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

};

بنية Drive_ثنائی_ہندسہ_مضرب
{
خاص:
	pool<Drive_ثنائی_ہندسہ> مضرب_3;

عام:
	Drive_ثنائی_ہندسہ_مضرب();
	Drive_ثنائی_ہندسہ_مضرب(Drive_ثنائی_ہندسہ ثابت &single);

	pool<Drive_ثنائی_ہندسہ> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	فراغ ملاؤ(Drive_ثنائی_ہندسہ_مضرب ثابت &other)
	{
		لكل (Drive_ثنائی_ہندسہ ثابت &single : other.مضرب_3)
			ملاؤ(single);
	}

	فراغ ملاؤ(Drive_ثنائی_ہندسہ_مضرب &&other)
	{
		لكل (Drive_ثنائی_ہندسہ &single : other.مضرب_3)
			ملاؤ(std::move(single));
	}

	فراغ ملاؤ(Drive_ثنائی_ہندسہ ثابت &single);
	فراغ ملاؤ(Drive_ثنائی_ہندسہ &&single);

	bool عامل==(ثابت Drive_ثنائی_ہندسہ_مضرب &other) ثابت
	{
		أرجع مضرب_3 == other.مضرب_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_ثنائی_ہندسہ
{
خاص:
	DriveType type_ = DriveType::NONE;
	اتحاد
	{
		State constant_;
		Drive_ثنائی_ہندسہ_Wire wire_;
		Drive_ثنائی_ہندسہ_پورٹ پورٹ_3;
		Drive_ثنائی_ہندسہ_Marker marker_;
		Drive_ثنائی_ہندسہ_مضرب مضرب_3;
	};
عام:
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

	فراغ سیٹ_none()
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				اكسر;
			حالة DriveType::CONSTANT:
				اكسر;
			حالة DriveType::WIRE:
				wire_.~Drive_ثنائی_ہندسہ_Wire();
				اكسر;
			حالة DriveType::پورٹ_2:
				پورٹ_3.~Drive_ثنائی_ہندسہ_پورٹ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~Drive_ثنائی_ہندسہ_Marker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_ثنائی_ہندسہ_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سیٹ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورٹ_2:
				*this = other.پورٹ_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::مضرب:
				*this = other.مضرب_3;
				اكسر;
		}
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سیٹ_none();
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورٹ_2:
				*this = std::move(other.پورٹ_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				اكسر;
		}
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(State constant)
	{
		سیٹ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Wire ثابت &wire)
	{
		سیٹ_none();
		new (&wire_) Drive_ثنائی_ہندسہ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Wire &&wire)
	{
		سیٹ_none();
		new (&wire_) Drive_ثنائی_ہندسہ_Wire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_پورٹ ثابت &پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_ثنائی_ہندسہ_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_پورٹ &&پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_ثنائی_ہندسہ_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Marker ثابت &marker)
	{
		سیٹ_none();
		new (&marker_) Drive_ثنائی_ہندسہ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_Marker &&marker)
	{
		سیٹ_none();
		new (&marker_) Drive_ثنائی_ہندسہ_Marker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_مضرب ثابت &مضرب_2)
	{
		سیٹ_none();
		إذا (مضرب_2.مضرب_2().خالی_تھاواں())
			أرجع *this;
		new (&مضرب_3) Drive_ثنائی_ہندسہ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	Drive_ثنائی_ہندسہ &عامل=(Drive_ثنائی_ہندسہ_مضرب &&مضرب_2)
	{
		سیٹ_none();
		إذا (مضرب_2.مضرب_2().خالی_تھاواں())
			أرجع *this;
		new (&مضرب_3) Drive_ثنائی_ہندسہ_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3 == other.پورٹ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عامل<(ثابت Drive_ثنائی_ہندسہ &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3 < other.پورٹ_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::مضرب:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورٹ() ثابت { أرجع type_ == DriveType::پورٹ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	State &constant() { log_assert(is_constant()); أرجع constant_; }
	State ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	Drive_ثنائی_ہندسہ_Wire &wire() { log_assert(is_wire()); أرجع wire_; }
	Drive_ثنائی_ہندسہ_Wire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_ثنائی_ہندسہ_پورٹ &پورٹ() { log_assert(is_پورٹ()); أرجع پورٹ_3; }
	Drive_ثنائی_ہندسہ_پورٹ ثابت &پورٹ() ثابت { log_assert(is_پورٹ()); أرجع پورٹ_3; }
	Drive_ثنائی_ہندسہ_Marker &marker() { log_assert(is_marker()); أرجع marker_; }
	Drive_ثنائی_ہندسہ_Marker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_ثنائی_ہندسہ_مضرب &مضرب_2() { log_assert(is_مضرب()); أرجع مضرب_3; }
	Drive_ثنائی_ہندسہ_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); أرجع مضرب_3; }

	فراغ ملاؤ(Drive_ثنائی_ہندسہ ثابت &other);

};

مضمن Drive_ثنائی_ہندسہ_مضرب::Drive_ثنائی_ہندسہ_مضرب() {}
مضمن Drive_ثنائی_ہندسہ_مضرب::Drive_ثنائی_ہندسہ_مضرب(Drive_ثنائی_ہندسہ ثابت &single)
{
	مضرب_3.emplace(single);
}

بنية DriveChunkWire
{
	Wire *wire;
	صحيح وتھ;
	صحيح چوڑائی;

	DriveChunkWire(Wire *wire, صحيح وتھ, صحيح چوڑائی) : wire(wire), وتھ(وتھ), چوڑائی(چوڑائی) {}
	DriveChunkWire(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) : wire(ثنائی_ہندسہ.wire), وتھ(ثنائی_ہندسہ.وتھ), چوڑائی(1) {}

	صحيح ناپ() ثابت { أرجع چوڑائی; }

	Drive_ثنائی_ہندسہ_Wire عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		أرجع Drive_ثنائی_ہندسہ_Wire(wire, وتھ + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunkWire ثابت &chunk);

	// Whether this chunk is a whole wire
	bool is_سارا() ثابت { أرجع وتھ == 0 && چوڑائی == wire->چوڑائی; }

	bool عامل==(ثابت DriveChunkWire &other) ثابت
	{
		أرجع wire == other.wire && وتھ == other.وتھ && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت DriveChunkWire &other) ثابت
	{
		إذا (wire != other.wire)
			أرجع wire->name < other.wire->name;
		إذا (چوڑائی != other.چوڑائی)
			أرجع چوڑائی < other.چوڑائی;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	explicit عامل SigChunk() ثابت
	{
		أرجع SigChunk(wire, وتھ, چوڑائی);
	}
};

بنية Drive_Chunk_پورٹ
{
	Cell *cell;
	IdString پورٹ;
	صحيح وتھ;
	صحيح چوڑائی;

	Drive_Chunk_پورٹ(Cell *cell, IdString پورٹ, صحيح وتھ, صحيح چوڑائی) :
		cell(cell), پورٹ(پورٹ), وتھ(وتھ), چوڑائی(چوڑائی) { }
	Drive_Chunk_پورٹ(Cell *cell, IdString پورٹ) :
		cell(cell), پورٹ(پورٹ), وتھ(0), چوڑائی(Get_ناپ(cell->connections().at(پورٹ))) { }
	Drive_Chunk_پورٹ(Cell *cell, std::pair<IdString, SigSpec> ثابت &conn) :
		cell(cell), پورٹ(conn.پہلا), وتھ(0), چوڑائی(Get_ناپ(conn.دوجا)) { }
	Drive_Chunk_پورٹ(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) :
		cell(ثنائی_ہندسہ.cell), پورٹ(ثنائی_ہندسہ.پورٹ), وتھ(ثنائی_ہندسہ.وتھ), چوڑائی(1) { }

	صحيح ناپ() ثابت { أرجع چوڑائی; }

	Drive_ثنائی_ہندسہ_پورٹ عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		أرجع Drive_ثنائی_ہندسہ_پورٹ(cell, پورٹ, وتھ + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ);
	bool try_append(Drive_Chunk_پورٹ ثابت &chunk);

	// Whether this chunk is a whole port
	bool is_سارا() ثابت
	{
		أرجع وتھ == 0 && چوڑائی == cell->connections().at(پورٹ).ناپ();
	}

	bool عامل==(ثابت Drive_Chunk_پورٹ &other) ثابت
	{
		أرجع cell == other.cell && پورٹ == other.پورٹ && وتھ == other.وتھ && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت Drive_Chunk_پورٹ &other) ثابت
	{
		إذا (cell != other.cell)
			أرجع cell->name < other.cell->name;
		إذا (پورٹ != other.پورٹ)
			أرجع پورٹ < other.پورٹ;
		إذا (چوڑائی != other.چوڑائی)
			أرجع چوڑائی < other.چوڑائی;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};


بنية DriveChunkMarker
{
	صحيح marker;
	صحيح وتھ;
	صحيح چوڑائی;

	DriveChunkMarker(صحيح marker, صحيح وتھ, صحيح چوڑائی) :
		marker(marker), وتھ(وتھ), چوڑائی(چوڑائی) {}
	DriveChunkMarker(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) :
		marker(ثنائی_ہندسہ.marker), وتھ(ثنائی_ہندسہ.وتھ), چوڑائی(1) {}

	صحيح ناپ() ثابت { أرجع چوڑائی; }

	Drive_ثنائی_ہندسہ_Marker عامل[](صحيح i) ثابت
	{
		log_assert(i >= 0 && i < چوڑائی);
		أرجع Drive_ثنائی_ہندسہ_Marker(marker, وتھ + i);
	}

	bool can_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunkMarker ثابت &chunk);

	bool عامل==(ثابت DriveChunkMarker &other) ثابت
	{
		أرجع marker == other.marker && وتھ == other.وتھ && چوڑائی == other.چوڑائی;
	}

	bool عامل<(ثابت DriveChunkMarker &other) ثابت
	{
		إذا (marker != other.marker)
			أرجع marker < other.marker;
		إذا (چوڑائی != other.چوڑائی)
			أرجع چوڑائی < other.چوڑائی;
		أرجع وتھ < other.وتھ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية Drive_Chunk_مضرب
{
خاص:
	mutable pool<DriveChunk> مضرب_3;
	صحيح چوڑائی_3;

عام:
	pool<DriveChunk> ثابت &مضرب_2() ثابت { أرجع مضرب_3; }

	Drive_Chunk_مضرب(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ);

	صحيح ناپ() ثابت { أرجع چوڑائی_3; }

	Drive_ثنائی_ہندسہ_مضرب عامل[](صحيح i) ثابت;

	bool can_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) ثابت;

	bool try_append(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ);


	bool can_append(Drive_Chunk_مضرب ثابت &ثنائی_ہندسہ) ثابت;

	bool try_append(Drive_Chunk_مضرب ثابت &ثنائی_ہندسہ);

	bool عامل==(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		أرجع چوڑائی_3 == other.چوڑائی_3 && مضرب_3 == other.مضرب_3;
	}

	bool عامل<(ثابت Drive_Chunk_مضرب &other) ثابت
	{
		إذا (مضرب_3.ناپ() < other.مضرب_3.ناپ())

		مضرب_3.ترتیب_دیو();
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
		Drive_Chunk_پورٹ پورٹ_3;
		DriveChunkMarker marker_;
		Drive_Chunk_مضرب مضرب_3;
	};

عام:
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

	Drive_ثنائی_ہندسہ عامل[](صحيح i) ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع Drive_ثنائی_ہندسہ();
			حالة DriveType::CONSTANT:
				أرجع constant_[i];
			حالة DriveType::WIRE:
				أرجع wire_[i];
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3[i];
			حالة DriveType::MARKER:
				أرجع marker_[i];
			حالة DriveType::مضرب:
				أرجع مضرب_3[i];
		}
		log_abort();
	}

	فراغ سیٹ_none(صحيح چوڑائی = 0)
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				none_ = چوڑائی;
				اكسر;
			حالة DriveType::CONSTANT:
				constant_.~Const();
				اكسر;
			حالة DriveType::WIRE:
				wire_.~DriveChunkWire();
				اكسر;
			حالة DriveType::پورٹ_2:
				پورٹ_3.~Drive_Chunk_پورٹ();
				اكسر;
			حالة DriveType::MARKER:
				marker_.~DriveChunkMarker();
				اكسر;
			حالة DriveType::مضرب:
				مضرب_3.~Drive_Chunk_مضرب();
				اكسر;
		}
		type_ = DriveType::NONE;
		none_ = چوڑائی;
	}

	DriveChunk &عامل=(DriveChunk ثابت &other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سیٹ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = other.constant_;
				اكسر;
			حالة DriveType::WIRE:
				*this = other.wire_;
				اكسر;
			حالة DriveType::پورٹ_2:
				*this = other.پورٹ_3;
				اكسر;
			حالة DriveType::MARKER:
				*this = other.marker_;
				اكسر;
			حالة DriveType::مضرب:
				*this = other.مضرب_3;
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunk &&other)
	{
		بدل (other.type_)
		{
			حالة DriveType::NONE:
				سیٹ_none(other.none_);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = std::move(other.constant_);
				اكسر;
			حالة DriveType::WIRE:
				*this = std::move(other.wire_);
				اكسر;
			حالة DriveType::پورٹ_2:
				*this = std::move(other.پورٹ_3);
				اكسر;
			حالة DriveType::MARKER:
				*this = std::move(other.marker_);
				اكسر;
			حالة DriveType::مضرب:
				*this = std::move(other.مضرب_3);
				اكسر;
		}
		أرجع *this;
	}

	DriveChunk &عامل=(Const ثابت &constant)
	{
		سیٹ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عامل=(Const &&constant)
	{
		سیٹ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkWire ثابت &wire)
	{
		سیٹ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkWire &&wire)
	{
		سیٹ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٹ ثابت &پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_Chunk_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_پورٹ &&پورٹ)
	{
		سیٹ_none();
		new (&پورٹ_3) Drive_Chunk_پورٹ(پورٹ);
		type_ = DriveType::پورٹ_2;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkMarker ثابت &marker)
	{
		سیٹ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عامل=(DriveChunkMarker &&marker)
	{
		سیٹ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_مضرب ثابت &مضرب_2)
	{
		سیٹ_none(مضرب_2.ناپ());
		إذا (مضرب_2.مضرب_2().خالی_تھاواں())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_Chunk_مضرب &&مضرب_2)
	{
		سیٹ_none(مضرب_2.ناپ());
		إذا (مضرب_2.مضرب_2().خالی_تھاواں())
			أرجع *this;
		new (&مضرب_3) Drive_Chunk_مضرب(مضرب_2);
		type_ = DriveType::مضرب;
		أرجع *this;
	}

	DriveChunk &عامل=(Drive_ثنائی_ہندسہ ثابت &other)
	{
		بدل (other.type())
		{
			حالة DriveType::NONE:
				سیٹ_none(1);
				اكسر;
			حالة DriveType::CONSTANT:
				*this = Const(other.constant());
				اكسر;
			حالة DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				اكسر;
			حالة DriveType::پورٹ_2:
				*this = Drive_Chunk_پورٹ(other.پورٹ());
				اكسر;
			حالة DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				اكسر;
			حالة DriveType::مضرب:
				*this = Drive_Chunk_مضرب(other.مضرب_2());
				اكسر;
		}
		أرجع *this;
	}

	bool can_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) ثابت;
	bool try_append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);
	bool try_append(DriveChunk ثابت &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(ثابت DriveChunk &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع false;

		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع true;
			حالة DriveType::CONSTANT:
				أرجع constant_ == other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ == other.wire_;
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3 == other.پورٹ_3;
			حالة DriveType::MARKER:
				أرجع marker_ == other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 == other.مضرب_3;
		}
		log_abort();
	}

	bool عامل!=(ثابت DriveChunk &other) ثابت
	{
		أرجع !(*this == other);
	}

	bool عامل<(ثابت DriveChunk &other) ثابت
	{
		إذا (type_ != other.type_)
			أرجع type_ < other.type_;
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع false;
			حالة DriveType::CONSTANT:
				أرجع constant_ < other.constant_;
			حالة DriveType::WIRE:
				أرجع wire_ < other.wire_;
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3 < other.پورٹ_3;
			حالة DriveType::MARKER:
				أرجع marker_ < other.marker_;
			حالة DriveType::مضرب:
				أرجع مضرب_3 < other.مضرب_3;
		}
		log_abort();
	}

	DriveType type() ثابت { أرجع type_; }

	bool is_none() ثابت { أرجع type_ == DriveType::NONE; }
	bool is_constant() ثابت { أرجع type_ == DriveType::CONSTANT; }
	bool is_wire() ثابت { أرجع type_ == DriveType::WIRE; }
	bool is_پورٹ() ثابت { أرجع type_ == DriveType::پورٹ_2; }
	bool is_marker() ثابت { أرجع type_ == DriveType::MARKER; }
	bool is_مضرب() ثابت { أرجع type_ == DriveType::مضرب; }

	Const &constant() { log_assert(is_constant()); أرجع constant_; }
	Const ثابت &constant() ثابت { log_assert(is_constant()); أرجع constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); أرجع wire_; }
	DriveChunkWire ثابت &wire() ثابت { log_assert(is_wire()); أرجع wire_; }
	Drive_Chunk_پورٹ &پورٹ() { log_assert(is_پورٹ()); أرجع پورٹ_3; }
	Drive_Chunk_پورٹ ثابت &پورٹ() ثابت { log_assert(is_پورٹ()); أرجع پورٹ_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); أرجع marker_; }
	DriveChunkMarker ثابت &marker() ثابت { log_assert(is_marker()); أرجع marker_; }
	Drive_Chunk_مضرب &مضرب_2() { log_assert(is_مضرب()); أرجع مضرب_3; }
	Drive_Chunk_مضرب ثابت &مضرب_2() ثابت { log_assert(is_مضرب()); أرجع مضرب_3; }


	صحيح ناپ() ثابت
	{
		بدل (type_)
		{
			حالة DriveType::NONE:
				أرجع none_;
			حالة DriveType::CONSTANT:
				أرجع constant_.ناپ();
			حالة DriveType::WIRE:
				أرجع wire_.ناپ();
			حالة DriveType::پورٹ_2:
				أرجع پورٹ_3.ناپ();
			حالة DriveType::MARKER:
				أرجع marker_.ناپ();
			حالة DriveType::مضرب:
				أرجع مضرب_3.ناپ();
		}
		log_abort();
	}
};

مضمن Drive_Chunk_مضرب::Drive_Chunk_مضرب(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ)
	: چوڑائی_3(1)
{
	لكل (خودکار ثابت &ثنائی_ہندسہ : ثنائی_ہندسہ.مضرب_2())
		مضرب_3.emplace(ثنائی_ہندسہ);
}

بنية DriveSpec
{
خاص:
	صحيح چوڑائی_3 = 0;
	mutable std::سمتیہ<DriveChunk> chunks_;
	mutable std::سمتیہ<Drive_ثنائی_ہندسہ> بٹ_2;
	mutable بنانشانی صحيح hash_ = 0;
عام:

	مضمن bool packed() ثابت {
		أرجع بٹ_2.خالی_تھاواں();
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
		لكل (خودکار ثابت &ثنائی_ہندسہ : بٹ)
			append(ثنائی_ہندسہ);
	}

	DriveSpec(SigSpec ثابت &sig)
	{
		// TODO: converting one chunk at a time would be faster
		لكل (خودکار ثابت &ثنائی_ہندسہ : sig.بٹ())
			append(ثنائی_ہندسہ);
	}

	std::سمتیہ<DriveChunk> ثابت &chunks() ثابت { pack(); أرجع chunks_; }
	std::سمتیہ<Drive_ثنائی_ہندسہ> ثابت &بٹ() ثابت { unpack(); أرجع بٹ_2; }

	صحيح ناپ() ثابت { أرجع چوڑائی_3; }

	فراغ append(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);

	فراغ append(DriveChunk ثابت &chunk);

	فراغ pack() ثابت;

	فراغ unpack() ثابت;

	Drive_ثنائی_ہندسہ &عامل[](صحيح اشاریہ)
	{
		log_assert(اشاریہ >= 0 && اشاریہ < ناپ());
		unpack();
		أرجع بٹ_2[اشاریہ];
	}

	ثابت Drive_ثنائی_ہندسہ &عامل[](صحيح اشاریہ) ثابت
	{
		log_assert(اشاریہ >= 0 && اشاریہ < ناپ());
		unpack();
		أرجع بٹ_2[اشاریہ];
	}

	فراغ clear()
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
		أرجع *this;
	}

	DriveSpec &عامل=(DriveChunkWire ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_پورٹ ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(DriveChunkMarker ثابت &chunk) { أرجع *this = DriveChunk(chunk); }
	DriveSpec &عامل=(Drive_Chunk_مضرب ثابت &chunk) { أرجع *this = DriveChunk(chunk); }

	DriveSpec &عامل=(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ)
	{
		chunks_.clear();
		بٹ_2.clear();
		append(ثنائی_ہندسہ);
		أرجع *this;
	}

	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_Wire ثابت &ثنائی_ہندسہ) { أرجع *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_پورٹ ثابت &ثنائی_ہندسہ) { أرجع *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_Marker ثابت &ثنائی_ہندسہ) { أرجع *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }
	DriveSpec &عامل=(Drive_ثنائی_ہندسہ_مضرب ثابت &ثنائی_ہندسہ) { أرجع *this = Drive_ثنائی_ہندسہ(ثنائی_ہندسہ); }

	فراغ updhash() ثابت {
		إذا (hash_ != 0)
			أرجع;
		pack();
		hash_ = لگاتار_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;

	bool عامل==(DriveSpec ثابت &other) ثابت {
		updhash();
		other.updhash();
		إذا (ناپ() != other.ناپ() || hash_ != other.hash_)
			أرجع false;
		أرجع chunks() == other.chunks();
	}

خاص:
	فراغ compute_چوڑائی();
};



بنية DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.تیاری(); }
	DriverMap(Design *design) { celltypes.تیاری(design); }

خاص:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	بنية Drive_ثنائی_ہندسہ_Id
	{
		صحيح id = -1;

		Drive_ثنائی_ہندسہ_Id() {};

		Drive_ثنائی_ہندسہ_Id(صحيح id) : id(id) { }

		bool عامل==(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { أرجع id == other.id; }
		bool عامل!=(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { أرجع id != other.id; }
		bool عامل<(ثابت Drive_ثنائی_ہندسہ_Id &other) ثابت { أرجع id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	بنية Drive_ثنائی_ہندسہ_گراف {
		dict<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ_Id> پہلا_کنارے;
		dict<Drive_ثنائی_ہندسہ_Id, Drive_ثنائی_ہندسہ_Id> دوجا_کنارے;
		dict<Drive_ثنائی_ہندسہ_Id, pool<Drive_ثنائی_ہندسہ_Id>> ہور_کنارے;

		فراغ جوڑو_کنارا(Drive_ثنائی_ہندسہ_Id src, Drive_ثنائی_ہندسہ_Id dst);
		Drive_ثنائی_ہندسہ_Id نکالو_کنارا(Drive_ثنائی_ہندسہ_Id src);
		فراغ clear(Drive_ثنائی_ہندسہ_Id src);
		bool شامل_اے(Drive_ثنائی_ہندسہ_Id src);
		صحيح گنتی(Drive_ثنائی_ہندسہ_Id src);

		Drive_ثنائی_ہندسہ_Id at(Drive_ثنائی_ہندسہ_Id src, صحيح اشاریہ);
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
	صحيح اگلا_وتھ = 1 + (صحيح)State::Sm;

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


	تعداد class ثنائی_ہندسہ_منوال {
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

	فراغ connect_directed_ملاؤ(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id);
	فراغ connect_directed_بفر(Drive_ثنائی_ہندسہ_Id driven_id, Drive_ثنائی_ہندسہ_Id driver_id);
	فراغ connect_undirected(Drive_ثنائی_ہندسہ_Id a_id, Drive_ثنائی_ہندسہ_Id b_id);

عام:

	فراغ جوڑو(Module *module);

	// Add a single bit connection to the driver map.
	فراغ جوڑو(Drive_ثنائی_ہندسہ ثابت &a, Drive_ثنائی_ہندسہ ثابت &b);

	سانچا<typename T>
	ساكن constexpr bool is_sig_type() {
		أرجع
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
	سانچا<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	جوڑو(T ثابت &a, U ثابت &b)
	{
		log_assert(a.ناپ() == b.ناپ());
		لكل (صحيح i = 0; i != Get_ناپ(a); ++i)
			جوڑو(Drive_ثنائی_ہندسہ(a[i]), Drive_ثنائی_ہندسہ(b[i]));
	}


	// Specialized version that avoids unpacking
	فراغ جوڑو(SigSpec ثابت &a, SigSpec ثابت &b);

خاص:
	فراغ جوڑو_پورٹ(Cell *cell, IdString پورٹ, SigSpec ثابت &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_ثنائی_ہندسہ_Id> orient_undirected_تکیا;
	pool<Drive_ثنائی_ہندسہ_Id> orient_undirected_drivers;
	dict<Drive_ثنائی_ہندسہ_Id, صحيح> orient_undirected_فاصلہ;

	فراغ orient_undirected(Drive_ثنائی_ہندسہ_Id id);

عام:
	Drive_ثنائی_ہندسہ عامل()(Drive_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ);

	DriveSpec عامل()(DriveSpec spec);

خاص:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		أرجع wire->has_attribute(ID(keep));
	}
};

مضمن Hasher Drive_ثنائی_ہندسہ_Wire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher Drive_ثنائی_ہندسہ_پورٹ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٹ);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher Drive_ثنائی_ہندسہ_Marker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher Drive_ثنائی_ہندسہ_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(مضرب_3);
	أرجع h;
}

مضمن Hasher Drive_ثنائی_ہندسہ::hash_into(Hasher h) ثابت
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
	حالة DriveType::پورٹ_2:
		h.eat(پورٹ_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::مضرب:
		h.eat(مضرب_3);
		اكسر;
	}
	h.eat(type_);
	أرجع h;
}

مضمن Hasher DriveChunkWire::hash_into(Hasher h) ثابت
{
	h.eat(wire->name);
	h.eat(چوڑائی);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher Drive_Chunk_پورٹ::hash_into(Hasher h) ثابت
{
	h.eat(cell->name);
	h.eat(پورٹ);
	h.eat(چوڑائی);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher DriveChunkMarker::hash_into(Hasher h) ثابت
{
	h.eat(marker);
	h.eat(چوڑائی);
	h.eat(وتھ);
	أرجع h;
}

مضمن Hasher Drive_Chunk_مضرب::hash_into(Hasher h) ثابت
{
	h.eat(چوڑائی_3);
	h.eat(مضرب_3);
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
	حالة DriveType::پورٹ_2:
		h.eat(پورٹ_3);
		اكسر;
	حالة DriveType::MARKER:
		h.eat(marker_);
		اكسر;
	حالة DriveType::مضرب:
		h.eat(مضرب_3);
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

مضمن Hasher DriverMap::Drive_ثنائی_ہندسہ_Id::hash_into(Hasher h) ثابت
{
	h.eat(id);
	أرجع h;
}

YOSYS_NAMESPACE_سرا

#جے_انت
