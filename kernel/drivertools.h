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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DRIVERTOOLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRIVERTOOLS_H

#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

// TODO move implementation into a .cc file

ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ;

ਬਣਤਰ DriveChunkWire;
ਬਣਤਰ Drive_Chunk_ਪੋਰਟ;
ਬਣਤਰ DriveChunkMarker;
ਬਣਤਰ DriveChunk;

ਬਣਤਰ DriveSpec;

std::string log_signal(DriveChunkWire ਅਚਲ &chunk);
std::string log_signal(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk);
std::string log_signal(DriveChunkMarker ਅਚਲ &chunk);
std::string log_signal(DriveChunk ਅਚਲ &chunk);
std::string log_signal(DriveSpec ਅਚਲ &chunk);

ਗਿਣਤੀ class DriveType : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ
{
	NONE,
	CONSTANT,
	WIRE,
	ਪੋਰਟ_2,
	ਗੁਣਜ,
	MARKER,
};

ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire
{
	Wire *wire;
	ਪੂਰਨ ਵਿੱਥ;

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(Wire *wire, ਪੂਰਨ ਵਿੱਥ) : wire(wire), ਵਿੱਥ(ਵਿੱਥ) {}

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire &other) ਅਚਲ
	{
		ਵਾਪਸ wire == other.wire && ਵਿੱਥ == other.ਵਿੱਥ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire &other) ਅਚਲ
	{
		ਜੇ (wire != other.wire)
			ਵਾਪਸ wire->name < other.wire->name;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;


	ਸੰਚਾਲਕ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ() ਅਚਲ
	{
		ਵਾਪਸ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, ਵਿੱਥ);
	}
};

ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ
{
	Cell *cell;
	IdString ਪੋਰਟ;
	ਪੂਰਨ ਵਿੱਥ;

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(Cell *cell, IdString ਪੋਰਟ, ਪੂਰਨ ਵਿੱਥ) : cell(cell), ਪੋਰਟ(ਪੋਰਟ), ਵਿੱਥ(ਵਿੱਥ) {}

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ &other) ਅਚਲ
	{
		ਵਾਪਸ cell == other.cell && ਪੋਰਟ == other.ਪੋਰਟ && ਵਿੱਥ == other.ਵਿੱਥ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ &other) ਅਚਲ
	{
		ਜੇ (cell != other.cell)
			ਵਾਪਸ cell->name < other.cell->name;
		ਜੇ (ਪੋਰਟ != other.ਪੋਰਟ)
			ਵਾਪਸ ਪੋਰਟ < other.ਪੋਰਟ;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

};


ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker
{
	ਪੂਰਨ marker;
	ਪੂਰਨ ਵਿੱਥ;

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker(ਪੂਰਨ marker, ਪੂਰਨ ਵਿੱਥ) : marker(marker), ਵਿੱਥ(ਵਿੱਥ) {}

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker &other) ਅਚਲ
	{
		ਵਾਪਸ marker == other.marker && ਵਿੱਥ == other.ਵਿੱਥ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker &other) ਅਚਲ
	{
		ਜੇ (marker != other.marker)
			ਵਾਪਸ marker < other.marker;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

};

ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ
{
ਨਿਜੀ:
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਗੁਣਜ_3;

ਜਨਤਕ:
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ();
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single);

	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &ਗੁਣਜ_2() ਅਚਲ { ਵਾਪਸ ਗੁਣਜ_3; }

	ਖਾਲੀ ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &other)
	{
		ਲਈ (Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single : other.ਗੁਣਜ_3)
			ਮਿਲਾਓ(single);
	}

	ਖਾਲੀ ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ &&other)
	{
		ਲਈ (Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &single : other.ਗੁਣਜ_3)
			ਮਿਲਾਓ(std::move(single));
	}

	ਖਾਲੀ ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single);
	ਖਾਲੀ ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &&single);

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ &other) ਅਚਲ
	{
		ਵਾਪਸ ਗੁਣਜ_3 == other.ਗੁਣਜ_3;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
};

ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ
{
ਨਿਜੀ:
	DriveType type_ = DriveType::NONE;
	ਸੰਘ
	{
		State constant_;
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire wire_;
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਪੋਰਟ_3;
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker marker_;
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਗੁਣਜ_3;
	};
ਜਨਤਕ:
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ()  {}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other) { *this = other; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &&other) { *this = other; }


	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(State constant) { *this = constant; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &wire) { *this = wire; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire &&wire) { *this = wire; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ) { *this = ਪੋਰਟ; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ &&ਪੋਰਟ) { *this = ਪੋਰਟ; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &marker) { *this = marker; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker &&marker) { *this = marker; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2) { *this = ਗੁਣਜ_2; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ &&ਗੁਣਜ_2) { *this = ਗੁਣਜ_2; }

	~Drive_ਦੋਆਧਾਰੀ_ਅੰਕ() { ਸੈੱਟ_none(); }

	ਖਾਲੀ ਸੈੱਟ_none()
	{
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				wire_.~Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਪੋਰਟ_3.~Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				marker_.~Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਗੁਣਜ_3.~Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ();
				ਰੋਕੋ;
		}
		type_ = DriveType::NONE;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other)
	{
		ਚੋਣ (other.type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਸੈੱਟ_none();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				*this = other.constant_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				*this = other.wire_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				*this = other.ਪੋਰਟ_3;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				*this = other.marker_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				*this = other.ਗੁਣਜ_3;
				ਰੋਕੋ;
		}
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &&other)
	{
		ਚੋਣ (other.type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਸੈੱਟ_none();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				*this = std::move(other.wire_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				*this = std::move(other.ਪੋਰਟ_3);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				*this = std::move(other.marker_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				*this = std::move(other.ਗੁਣਜ_3);
				ਰੋਕੋ;
		}
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(State constant)
	{
		ਸੈੱਟ_none();
		constant_ = constant;
		type_ = DriveType::CONSTANT;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &wire)
	{
		ਸੈੱਟ_none();
		new (&wire_) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(wire);
		type_ = DriveType::WIRE;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire &&wire)
	{
		ਸੈੱਟ_none();
		new (&wire_) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(wire);
		type_ = DriveType::WIRE;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ)
	{
		ਸੈੱਟ_none();
		new (&ਪੋਰਟ_3) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(ਪੋਰਟ);
		type_ = DriveType::ਪੋਰਟ_2;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ &&ਪੋਰਟ)
	{
		ਸੈੱਟ_none();
		new (&ਪੋਰਟ_3) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(ਪੋਰਟ);
		type_ = DriveType::ਪੋਰਟ_2;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &marker)
	{
		ਸੈੱਟ_none();
		new (&marker_) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker(marker);
		type_ = DriveType::MARKER;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker &&marker)
	{
		ਸੈੱਟ_none();
		new (&marker_) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker(marker);
		type_ = DriveType::MARKER;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2)
	{
		ਸੈੱਟ_none();
		ਜੇ (ਗੁਣਜ_2.ਗੁਣਜ_2().ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ *this;
		new (&ਗੁਣਜ_3) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ(ਗੁਣਜ_2);
		type_ = DriveType::ਗੁਣਜ;
		ਵਾਪਸ *this;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ &&ਗੁਣਜ_2)
	{
		ਸੈੱਟ_none();
		ਜੇ (ਗੁਣਜ_2.ਗੁਣਜ_2().ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ *this;
		new (&ਗੁਣਜ_3) Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ(ਗੁਣਜ_2);
		type_ = DriveType::ਗੁਣਜ;
		ਵਾਪਸ *this;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ
	{
		ਜੇ (type_ != other.type_)
			ਵਾਪਸ false;

		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ true;
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_ == other.constant_;
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_ == other.wire_;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3 == other.ਪੋਰਟ_3;
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_ == other.marker_;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਵਾਪਸ ਗੁਣਜ_3 == other.ਗੁਣਜ_3;
		}
		log_abort();
	}

	bool ਸੰਚਾਲਕ!=(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ
	{
		ਵਾਪਸ !(*this == other);
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ
	{
		ਜੇ (type_ != other.type_)
			ਵਾਪਸ type_ < other.type_;
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ false;
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_ < other.constant_;
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_ < other.wire_;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3 < other.ਪੋਰਟ_3;
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_ < other.marker_;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				log_assert(!"TODO");
		}
		log_abort();
	}


	DriveType type() ਅਚਲ { ਵਾਪਸ type_; }

	bool is_none() ਅਚਲ { ਵਾਪਸ type_ == DriveType::NONE; }
	bool is_constant() ਅਚਲ { ਵਾਪਸ type_ == DriveType::CONSTANT; }
	bool is_wire() ਅਚਲ { ਵਾਪਸ type_ == DriveType::WIRE; }
	bool is_ਪੋਰਟ() ਅਚਲ { ਵਾਪਸ type_ == DriveType::ਪੋਰਟ_2; }
	bool is_marker() ਅਚਲ { ਵਾਪਸ type_ == DriveType::MARKER; }
	bool is_ਗੁਣਜ() ਅਚਲ { ਵਾਪਸ type_ == DriveType::ਗੁਣਜ; }

	State &constant() { log_assert(is_constant()); ਵਾਪਸ constant_; }
	State ਅਚਲ &constant() ਅਚਲ { log_assert(is_constant()); ਵਾਪਸ constant_; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire &wire() { log_assert(is_wire()); ਵਾਪਸ wire_; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &wire() ਅਚਲ { log_assert(is_wire()); ਵਾਪਸ wire_; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ &ਪੋਰਟ() { log_assert(is_ਪੋਰਟ()); ਵਾਪਸ ਪੋਰਟ_3; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ() ਅਚਲ { log_assert(is_ਪੋਰਟ()); ਵਾਪਸ ਪੋਰਟ_3; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker &marker() { log_assert(is_marker()); ਵਾਪਸ marker_; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &marker() ਅਚਲ { log_assert(is_marker()); ਵਾਪਸ marker_; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ &ਗੁਣਜ_2() { log_assert(is_ਗੁਣਜ()); ਵਾਪਸ ਗੁਣਜ_3; }
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2() ਅਚਲ { log_assert(is_ਗੁਣਜ()); ਵਾਪਸ ਗੁਣਜ_3; }

	ਖਾਲੀ ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other);

};

ਸਤਰਅੰਦਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ() {}
ਸਤਰਅੰਦਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single)
{
	ਗੁਣਜ_3.emplace(single);
}

ਬਣਤਰ DriveChunkWire
{
	Wire *wire;
	ਪੂਰਨ ਵਿੱਥ;
	ਪੂਰਨ ਚੌੜਾਈ;

	DriveChunkWire(Wire *wire, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ) : wire(wire), ਵਿੱਥ(ਵਿੱਥ), ਚੌੜਾਈ(ਚੌੜਾਈ) {}
	DriveChunkWire(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) : wire(ਦੋਆਧਾਰੀ_ਅੰਕ.wire), ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ), ਚੌੜਾਈ(1) {}

	ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ; }

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ
	{
		log_assert(i >= 0 && i < ਚੌੜਾਈ);
		ਵਾਪਸ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(wire, ਵਿੱਥ + i);
	}

	bool can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;
	bool try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	bool try_append(DriveChunkWire ਅਚਲ &chunk);

	// Whether this chunk is a whole wire
	bool is_ਸਾਰਾ() ਅਚਲ { ਵਾਪਸ ਵਿੱਥ == 0 && ਚੌੜਾਈ == wire->ਚੌੜਾਈ; }

	bool ਸੰਚਾਲਕ==(ਅਚਲ DriveChunkWire &other) ਅਚਲ
	{
		ਵਾਪਸ wire == other.wire && ਵਿੱਥ == other.ਵਿੱਥ && ਚੌੜਾਈ == other.ਚੌੜਾਈ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ DriveChunkWire &other) ਅਚਲ
	{
		ਜੇ (wire != other.wire)
			ਵਾਪਸ wire->name < other.wire->name;
		ਜੇ (ਚੌੜਾਈ != other.ਚੌੜਾਈ)
			ਵਾਪਸ ਚੌੜਾਈ < other.ਚੌੜਾਈ;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

	explicit ਸੰਚਾਲਕ SigChunk() ਅਚਲ
	{
		ਵਾਪਸ SigChunk(wire, ਵਿੱਥ, ਚੌੜਾਈ);
	}
};

ਬਣਤਰ Drive_Chunk_ਪੋਰਟ
{
	Cell *cell;
	IdString ਪੋਰਟ;
	ਪੂਰਨ ਵਿੱਥ;
	ਪੂਰਨ ਚੌੜਾਈ;

	Drive_Chunk_ਪੋਰਟ(Cell *cell, IdString ਪੋਰਟ, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ) :
		cell(cell), ਪੋਰਟ(ਪੋਰਟ), ਵਿੱਥ(ਵਿੱਥ), ਚੌੜਾਈ(ਚੌੜਾਈ) { }
	Drive_Chunk_ਪੋਰਟ(Cell *cell, IdString ਪੋਰਟ) :
		cell(cell), ਪੋਰਟ(ਪੋਰਟ), ਵਿੱਥ(0), ਚੌੜਾਈ(Get_ਮਾਪ(cell->connections().at(ਪੋਰਟ))) { }
	Drive_Chunk_ਪੋਰਟ(Cell *cell, std::pair<IdString, SigSpec> ਅਚਲ &conn) :
		cell(cell), ਪੋਰਟ(conn.ਪਹਿਲਾ), ਵਿੱਥ(0), ਚੌੜਾਈ(Get_ਮਾਪ(conn.ਦੂਜਾ)) { }
	Drive_Chunk_ਪੋਰਟ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) :
		cell(ਦੋਆਧਾਰੀ_ਅੰਕ.cell), ਪੋਰਟ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ), ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ), ਚੌੜਾਈ(1) { }

	ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ; }

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ
	{
		log_assert(i >= 0 && i < ਚੌੜਾਈ);
		ਵਾਪਸ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(cell, ਪੋਰਟ, ਵਿੱਥ + i);
	}

	bool can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;
	bool try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	bool try_append(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk);

	// Whether this chunk is a whole port
	bool is_ਸਾਰਾ() ਅਚਲ
	{
		ਵਾਪਸ ਵਿੱਥ == 0 && ਚੌੜਾਈ == cell->connections().at(ਪੋਰਟ).ਮਾਪ();
	}

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_Chunk_ਪੋਰਟ &other) ਅਚਲ
	{
		ਵਾਪਸ cell == other.cell && ਪੋਰਟ == other.ਪੋਰਟ && ਵਿੱਥ == other.ਵਿੱਥ && ਚੌੜਾਈ == other.ਚੌੜਾਈ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_Chunk_ਪੋਰਟ &other) ਅਚਲ
	{
		ਜੇ (cell != other.cell)
			ਵਾਪਸ cell->name < other.cell->name;
		ਜੇ (ਪੋਰਟ != other.ਪੋਰਟ)
			ਵਾਪਸ ਪੋਰਟ < other.ਪੋਰਟ;
		ਜੇ (ਚੌੜਾਈ != other.ਚੌੜਾਈ)
			ਵਾਪਸ ਚੌੜਾਈ < other.ਚੌੜਾਈ;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
};


ਬਣਤਰ DriveChunkMarker
{
	ਪੂਰਨ marker;
	ਪੂਰਨ ਵਿੱਥ;
	ਪੂਰਨ ਚੌੜਾਈ;

	DriveChunkMarker(ਪੂਰਨ marker, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ) :
		marker(marker), ਵਿੱਥ(ਵਿੱਥ), ਚੌੜਾਈ(ਚੌੜਾਈ) {}
	DriveChunkMarker(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) :
		marker(ਦੋਆਧਾਰੀ_ਅੰਕ.marker), ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ), ਚੌੜਾਈ(1) {}

	ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ; }

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ
	{
		log_assert(i >= 0 && i < ਚੌੜਾਈ);
		ਵਾਪਸ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker(marker, ਵਿੱਥ + i);
	}

	bool can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;
	bool try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	bool try_append(DriveChunkMarker ਅਚਲ &chunk);

	bool ਸੰਚਾਲਕ==(ਅਚਲ DriveChunkMarker &other) ਅਚਲ
	{
		ਵਾਪਸ marker == other.marker && ਵਿੱਥ == other.ਵਿੱਥ && ਚੌੜਾਈ == other.ਚੌੜਾਈ;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ DriveChunkMarker &other) ਅਚਲ
	{
		ਜੇ (marker != other.marker)
			ਵਾਪਸ marker < other.marker;
		ਜੇ (ਚੌੜਾਈ != other.ਚੌੜਾਈ)
			ਵਾਪਸ ਚੌੜਾਈ < other.ਚੌੜਾਈ;
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
};

ਬਣਤਰ Drive_Chunk_ਗੁਣਜ
{
ਨਿਜੀ:
	mutable pool<DriveChunk> ਗੁਣਜ_3;
	ਪੂਰਨ ਚੌੜਾਈ_3;

ਜਨਤਕ:
	pool<DriveChunk> ਅਚਲ &ਗੁਣਜ_2() ਅਚਲ { ਵਾਪਸ ਗੁਣਜ_3; }

	Drive_Chunk_ਗੁਣਜ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ_3; }

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ;

	bool can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;

	bool try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);


	bool can_append(Drive_Chunk_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;

	bool try_append(Drive_Chunk_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_Chunk_ਗੁਣਜ &other) ਅਚਲ
	{
		ਵਾਪਸ ਚੌੜਾਈ_3 == other.ਚੌੜਾਈ_3 && ਗੁਣਜ_3 == other.ਗੁਣਜ_3;
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_Chunk_ਗੁਣਜ &other) ਅਚਲ
	{
		ਜੇ (ਗੁਣਜ_3.ਮਾਪ() < other.ਗੁਣਜ_3.ਮਾਪ())

		ਗੁਣਜ_3.ਛਾਂਟੋ();
		ਵਾਪਸ false; // TODO implement, canonicalize order
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
};

ਬਣਤਰ DriveChunk
{
ਨਿਜੀ:
	DriveType type_ = DriveType::NONE;
	ਸੰਘ
	{
		ਪੂਰਨ none_;
		Const constant_;
		DriveChunkWire wire_;
		Drive_Chunk_ਪੋਰਟ ਪੋਰਟ_3;
		DriveChunkMarker marker_;
		Drive_Chunk_ਗੁਣਜ ਗੁਣਜ_3;
	};

ਜਨਤਕ:
	DriveChunk() { ਸੈੱਟ_none(); }

	DriveChunk(DriveChunk ਅਚਲ &other) { *this = other; }
	DriveChunk(DriveChunk &&other) { *this = other; }

	DriveChunk(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other) { *this = other; }

	DriveChunk(Const ਅਚਲ &constant) { *this = constant; }
	DriveChunk(Const &&constant) { *this = constant; }
	DriveChunk(DriveChunkWire ਅਚਲ &wire) { *this = wire; }
	DriveChunk(DriveChunkWire &&wire) { *this = wire; }
	DriveChunk(Drive_Chunk_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ) { *this = ਪੋਰਟ; }
	DriveChunk(Drive_Chunk_ਪੋਰਟ &&ਪੋਰਟ) { *this = ਪੋਰਟ; }
	DriveChunk(DriveChunkMarker ਅਚਲ &marker) { *this = marker; }
	DriveChunk(DriveChunkMarker &&marker) { *this = marker; }
	DriveChunk(Drive_Chunk_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2) { *this = ਗੁਣਜ_2; }
	DriveChunk(Drive_Chunk_ਗੁਣਜ &&ਗੁਣਜ_2) { *this = ਗੁਣਜ_2; }

	~DriveChunk() { ਸੈੱਟ_none(); }

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ
	{
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ();
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_[i];
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_[i];
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3[i];
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_[i];
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਵਾਪਸ ਗੁਣਜ_3[i];
		}
		log_abort();
	}

	ਖਾਲੀ ਸੈੱਟ_none(ਪੂਰਨ ਚੌੜਾਈ = 0)
	{
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				none_ = ਚੌੜਾਈ;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				constant_.~Const();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				wire_.~DriveChunkWire();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਪੋਰਟ_3.~Drive_Chunk_ਪੋਰਟ();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				marker_.~DriveChunkMarker();
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਗੁਣਜ_3.~Drive_Chunk_ਗੁਣਜ();
				ਰੋਕੋ;
		}
		type_ = DriveType::NONE;
		none_ = ਚੌੜਾਈ;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunk ਅਚਲ &other)
	{
		ਚੋਣ (other.type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਸੈੱਟ_none(other.none_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				*this = other.constant_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				*this = other.wire_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				*this = other.ਪੋਰਟ_3;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				*this = other.marker_;
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				*this = other.ਗੁਣਜ_3;
				ਰੋਕੋ;
		}
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunk &&other)
	{
		ਚੋਣ (other.type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਸੈੱਟ_none(other.none_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				*this = std::move(other.constant_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				*this = std::move(other.wire_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				*this = std::move(other.ਪੋਰਟ_3);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				*this = std::move(other.marker_);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				*this = std::move(other.ਗੁਣਜ_3);
				ਰੋਕੋ;
		}
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Const ਅਚਲ &constant)
	{
		ਸੈੱਟ_none();
		new (&constant_) Const(constant);
		type_ = DriveType::CONSTANT;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Const &&constant)
	{
		ਸੈੱਟ_none();
		new (&constant_) Const(std::move(constant));
		type_ = DriveType::CONSTANT;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunkWire ਅਚਲ &wire)
	{
		ਸੈੱਟ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunkWire &&wire)
	{
		ਸੈੱਟ_none();
		new (&wire_) DriveChunkWire(wire);
		type_ = DriveType::WIRE;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Drive_Chunk_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ)
	{
		ਸੈੱਟ_none();
		new (&ਪੋਰਟ_3) Drive_Chunk_ਪੋਰਟ(ਪੋਰਟ);
		type_ = DriveType::ਪੋਰਟ_2;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Drive_Chunk_ਪੋਰਟ &&ਪੋਰਟ)
	{
		ਸੈੱਟ_none();
		new (&ਪੋਰਟ_3) Drive_Chunk_ਪੋਰਟ(ਪੋਰਟ);
		type_ = DriveType::ਪੋਰਟ_2;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunkMarker ਅਚਲ &marker)
	{
		ਸੈੱਟ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(DriveChunkMarker &&marker)
	{
		ਸੈੱਟ_none();
		new (&marker_) DriveChunkMarker(marker);
		type_ = DriveType::MARKER;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Drive_Chunk_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2)
	{
		ਸੈੱਟ_none(ਗੁਣਜ_2.ਮਾਪ());
		ਜੇ (ਗੁਣਜ_2.ਗੁਣਜ_2().ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ *this;
		new (&ਗੁਣਜ_3) Drive_Chunk_ਗੁਣਜ(ਗੁਣਜ_2);
		type_ = DriveType::ਗੁਣਜ;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Drive_Chunk_ਗੁਣਜ &&ਗੁਣਜ_2)
	{
		ਸੈੱਟ_none(ਗੁਣਜ_2.ਮਾਪ());
		ਜੇ (ਗੁਣਜ_2.ਗੁਣਜ_2().ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ *this;
		new (&ਗੁਣਜ_3) Drive_Chunk_ਗੁਣਜ(ਗੁਣਜ_2);
		type_ = DriveType::ਗੁਣਜ;
		ਵਾਪਸ *this;
	}

	DriveChunk &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other)
	{
		ਚੋਣ (other.type())
		{
			ਹਾਲਤ DriveType::NONE:
				ਸੈੱਟ_none(1);
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::CONSTANT:
				*this = Const(other.constant());
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE:
				*this = DriveChunkWire(other.wire());
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				*this = Drive_Chunk_ਪੋਰਟ(other.ਪੋਰਟ());
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER:
				*this = DriveChunkMarker(other.marker());
				ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				*this = Drive_Chunk_ਗੁਣਜ(other.ਗੁਣਜ_2());
				ਰੋਕੋ;
		}
		ਵਾਪਸ *this;
	}

	bool can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ;
	bool try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	bool try_append(DriveChunk ਅਚਲ &chunk);

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

	bool ਸੰਚਾਲਕ==(ਅਚਲ DriveChunk &other) ਅਚਲ
	{
		ਜੇ (type_ != other.type_)
			ਵਾਪਸ false;

		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ true;
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_ == other.constant_;
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_ == other.wire_;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3 == other.ਪੋਰਟ_3;
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_ == other.marker_;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਵਾਪਸ ਗੁਣਜ_3 == other.ਗੁਣਜ_3;
		}
		log_abort();
	}

	bool ਸੰਚਾਲਕ!=(ਅਚਲ DriveChunk &other) ਅਚਲ
	{
		ਵਾਪਸ !(*this == other);
	}

	bool ਸੰਚਾਲਕ<(ਅਚਲ DriveChunk &other) ਅਚਲ
	{
		ਜੇ (type_ != other.type_)
			ਵਾਪਸ type_ < other.type_;
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ false;
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_ < other.constant_;
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_ < other.wire_;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3 < other.ਪੋਰਟ_3;
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_ < other.marker_;
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਵਾਪਸ ਗੁਣਜ_3 < other.ਗੁਣਜ_3;
		}
		log_abort();
	}

	DriveType type() ਅਚਲ { ਵਾਪਸ type_; }

	bool is_none() ਅਚਲ { ਵਾਪਸ type_ == DriveType::NONE; }
	bool is_constant() ਅਚਲ { ਵਾਪਸ type_ == DriveType::CONSTANT; }
	bool is_wire() ਅਚਲ { ਵਾਪਸ type_ == DriveType::WIRE; }
	bool is_ਪੋਰਟ() ਅਚਲ { ਵਾਪਸ type_ == DriveType::ਪੋਰਟ_2; }
	bool is_marker() ਅਚਲ { ਵਾਪਸ type_ == DriveType::MARKER; }
	bool is_ਗੁਣਜ() ਅਚਲ { ਵਾਪਸ type_ == DriveType::ਗੁਣਜ; }

	Const &constant() { log_assert(is_constant()); ਵਾਪਸ constant_; }
	Const ਅਚਲ &constant() ਅਚਲ { log_assert(is_constant()); ਵਾਪਸ constant_; }
	DriveChunkWire &wire() { log_assert(is_wire()); ਵਾਪਸ wire_; }
	DriveChunkWire ਅਚਲ &wire() ਅਚਲ { log_assert(is_wire()); ਵਾਪਸ wire_; }
	Drive_Chunk_ਪੋਰਟ &ਪੋਰਟ() { log_assert(is_ਪੋਰਟ()); ਵਾਪਸ ਪੋਰਟ_3; }
	Drive_Chunk_ਪੋਰਟ ਅਚਲ &ਪੋਰਟ() ਅਚਲ { log_assert(is_ਪੋਰਟ()); ਵਾਪਸ ਪੋਰਟ_3; }
	DriveChunkMarker &marker() { log_assert(is_marker()); ਵਾਪਸ marker_; }
	DriveChunkMarker ਅਚਲ &marker() ਅਚਲ { log_assert(is_marker()); ਵਾਪਸ marker_; }
	Drive_Chunk_ਗੁਣਜ &ਗੁਣਜ_2() { log_assert(is_ਗੁਣਜ()); ਵਾਪਸ ਗੁਣਜ_3; }
	Drive_Chunk_ਗੁਣਜ ਅਚਲ &ਗੁਣਜ_2() ਅਚਲ { log_assert(is_ਗੁਣਜ()); ਵਾਪਸ ਗੁਣਜ_3; }


	ਪੂਰਨ ਮਾਪ() ਅਚਲ
	{
		ਚੋਣ (type_)
		{
			ਹਾਲਤ DriveType::NONE:
				ਵਾਪਸ none_;
			ਹਾਲਤ DriveType::CONSTANT:
				ਵਾਪਸ constant_.ਮਾਪ();
			ਹਾਲਤ DriveType::WIRE:
				ਵਾਪਸ wire_.ਮਾਪ();
			ਹਾਲਤ DriveType::ਪੋਰਟ_2:
				ਵਾਪਸ ਪੋਰਟ_3.ਮਾਪ();
			ਹਾਲਤ DriveType::MARKER:
				ਵਾਪਸ marker_.ਮਾਪ();
			ਹਾਲਤ DriveType::ਗੁਣਜ:
				ਵਾਪਸ ਗੁਣਜ_3.ਮਾਪ();
		}
		log_abort();
	}
};

ਸਤਰਅੰਦਰ Drive_Chunk_ਗੁਣਜ::Drive_Chunk_ਗੁਣਜ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
	: ਚੌੜਾਈ_3(1)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2())
		ਗੁਣਜ_3.emplace(ਦੋਆਧਾਰੀ_ਅੰਕ);
}

ਬਣਤਰ DriveSpec
{
ਨਿਜੀ:
	ਪੂਰਨ ਚੌੜਾਈ_3 = 0;
	mutable std::ਸਦਿਸ਼<DriveChunk> chunks_;
	mutable std::ਸਦਿਸ਼<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_2;
	mutable ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hash_ = 0;
ਜਨਤਕ:

	ਸਤਰਅੰਦਰ bool packed() ਅਚਲ {
		ਵਾਪਸ ਬਿੱਟ_2.ਖਾਲੀ_ਥਾਵਾਂ();
	}

	DriveSpec() {}

	DriveSpec(DriveChunk ਅਚਲ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkWire ਅਚਲ &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk) { *this = chunk; }
	DriveSpec(DriveChunkMarker ਅਚਲ &chunk) { *this = chunk; }
	DriveSpec(Drive_Chunk_ਗੁਣਜ ਅਚਲ &chunk) { *this = chunk; }

	DriveSpec(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { *this = ਦੋਆਧਾਰੀ_ਅੰਕ; }
	DriveSpec(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { *this = ਦੋਆਧਾਰੀ_ਅੰਕ; }
	DriveSpec(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { *this = ਦੋਆਧਾਰੀ_ਅੰਕ; }
	DriveSpec(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { *this = ਦੋਆਧਾਰੀ_ਅੰਕ; }
	DriveSpec(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { *this = ਦੋਆਧਾਰੀ_ਅੰਕ; }

	DriveSpec(std::ਸਦਿਸ਼<DriveChunk> ਅਚਲ &chunks) : chunks_(chunks) { compute_ਚੌੜਾਈ(); }

	DriveSpec(std::ਸਦਿਸ਼<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &ਬਿੱਟ)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	DriveSpec(SigSpec ਅਚਲ &sig)
	{
		// TODO: converting one chunk at a time would be faster
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig.ਬਿੱਟ())
			append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	std::ਸਦਿਸ਼<DriveChunk> ਅਚਲ &chunks() ਅਚਲ { pack(); ਵਾਪਸ chunks_; }
	std::ਸਦਿਸ਼<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਚਲ &ਬਿੱਟ() ਅਚਲ { unpack(); ਵਾਪਸ ਬਿੱਟ_2; }

	ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ_3; }

	ਖਾਲੀ append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	ਖਾਲੀ append(DriveChunk ਅਚਲ &chunk);

	ਖਾਲੀ pack() ਅਚਲ;

	ਖਾਲੀ unpack() ਅਚਲ;

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ)
	{
		log_assert(ਸੂਚਕਾਂਕ >= 0 && ਸੂਚਕਾਂਕ < ਮਾਪ());
		unpack();
		ਵਾਪਸ ਬਿੱਟ_2[ਸੂਚਕਾਂਕ];
	}

	ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ
	{
		log_assert(ਸੂਚਕਾਂਕ >= 0 && ਸੂਚਕਾਂਕ < ਮਾਪ());
		unpack();
		ਵਾਪਸ ਬਿੱਟ_2[ਸੂਚਕਾਂਕ];
	}

	ਖਾਲੀ clear()
	{
		chunks_.clear();
		ਬਿੱਟ_2.clear();
		ਚੌੜਾਈ_3 = 0;
	}

	DriveSpec &ਸੰਚਾਲਕ=(DriveChunk ਅਚਲ &chunk)
	{
		chunks_.clear();
		ਬਿੱਟ_2.clear();
		append(chunk);
		ਵਾਪਸ *this;
	}

	DriveSpec &ਸੰਚਾਲਕ=(DriveChunkWire ਅਚਲ &chunk) { ਵਾਪਸ *this = DriveChunk(chunk); }
	DriveSpec &ਸੰਚਾਲਕ=(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk) { ਵਾਪਸ *this = DriveChunk(chunk); }
	DriveSpec &ਸੰਚਾਲਕ=(DriveChunkMarker ਅਚਲ &chunk) { ਵਾਪਸ *this = DriveChunk(chunk); }
	DriveSpec &ਸੰਚਾਲਕ=(Drive_Chunk_ਗੁਣਜ ਅਚਲ &chunk) { ਵਾਪਸ *this = DriveChunk(chunk); }

	DriveSpec &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		chunks_.clear();
		ਬਿੱਟ_2.clear();
		append(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ *this;
	}

	DriveSpec &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { ਵਾਪਸ *this = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ); }
	DriveSpec &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { ਵਾਪਸ *this = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ); }
	DriveSpec &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { ਵਾਪਸ *this = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ); }
	DriveSpec &ਸੰਚਾਲਕ=(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { ਵਾਪਸ *this = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ); }

	ਖਾਲੀ updhash() ਅਚਲ {
		ਜੇ (hash_ != 0)
			ਵਾਪਸ;
		pack();
		hash_ = ਲੜੀਵਾਰ_hash(chunks_);
		hash_ |= (hash_ == 0);
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;

	bool ਸੰਚਾਲਕ==(DriveSpec ਅਚਲ &other) ਅਚਲ {
		updhash();
		other.updhash();
		ਜੇ (ਮਾਪ() != other.ਮਾਪ() || hash_ != other.hash_)
			ਵਾਪਸ false;
		ਵਾਪਸ chunks() == other.chunks();
	}

ਨਿਜੀ:
	ਖਾਲੀ compute_ਚੌੜਾਈ();
};



ਬਣਤਰ DriverMap
{
	NewCellTypes celltypes;

	DriverMap() { celltypes.ਤਿਆਰੀ(); }
	DriverMap(Design *design) { celltypes.ਤਿਆਰੀ(design); }

ਨਿਜੀ:

	// Internally we represent all DriveBits by mapping them to DriveBitIds
	// which use less memory and are cheaper to compare.
	ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id
	{
		ਪੂਰਨ id = -1;

		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id() {};

		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id(ਪੂਰਨ id) : id(id) { }

		bool ਸੰਚਾਲਕ==(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id &other) ਅਚਲ { ਵਾਪਸ id == other.id; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id &other) ਅਚਲ { ਵਾਪਸ id != other.id; }
		bool ਸੰਚਾਲਕ<(ਅਚਲ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id &other) ਅਚਲ { ਵਾਪਸ id < other.id; }
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
	};
	// Essentially a dict<DriveBitId, pool<DriveBitId>> but using less memory
	// and fewer allocations
	ਬਣਤਰ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ {
		dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> ਪਹਿਲਾ_ਕਿਨਾਰੇ;
		dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> ਦੂਜਾ_ਕਿਨਾਰੇ;
		dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id>> ਹੋਰ_ਕਿਨਾਰੇ;

		ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id dst);
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id ਕੱਢੋ_ਕਿਨਾਰਾ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src);
		ਖਾਲੀ clear(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src);
		bool ਸ਼ਾਮਲ_ਹੈ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src);
		ਪੂਰਨ ਗਿਣਤੀ_2(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src);

		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id at(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src, ਪੂਰਨ ਸੂਚਕਾਂਕ);
	};

	// The following two maps maintain a sparse DriveBit to DriveBitId mapping.
	// This saves a lot of memory compared to a `dict<DriveBit, DriveBitId>` or
	// `idict<DriveBit>`.

	// Maps wires to the first DriveBitId of the consecutive range used for
	// that wire.
	dict<Wire *, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> wire_offsets;

	// Maps cell ports to a the first DriveBitId of the consecutive range used
	// for that cell port.
	dict<pair<Cell *, IdString>, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> ਪੋਰਟ_offsets;

	// For the inverse map that maps DriveBitIds back to DriveBits we use a
	// sorted map containing only the first DriveBit for each wire and cell
	// port.
	std::map<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> drive_ਬਿੱਟ;

	// As a memory optimization for gate level net lists we store single-bit
	// wires and cell ports in a `dict` which requires less memory and fewer
	// allocations than `std::map` but doesn't support the kind of lookups we
	// need for a sparse coarse grained mapping.
	dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> isolated_drive_ਬਿੱਟ;

	// Used for allocating DriveBitIds, none and constant states use a fixewd
	// mapping to the first few ids, which we need to skip.
	ਪੂਰਨ ਅਗਲਾ_ਵਿੱਥ = 1 + (ਪੂਰਨ)State::Sm;

	// Union-Find over directly connected bits that share the same single
	// driver or are undriven. We never merge connections between drivers
	// and/or kept wires.
	mfp<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> same_driver;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally unambiguous (one side only drives or requires a driven value).
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ connected_drivers;

	// For each bit, store a set of connected driver bits for which the
	// explicit connection should be preserved and the driving direction is
	// locally ambiguous. Every such ambiguous connection is also present in
	// the reverse direction and has to be resolved when querying drivers.
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ connected_undirected;

	// Subset of `connected_undirected` for caching the resolved driving
	// direction. In case multiple drivers are present this can still contain
	// both orientations of a single connection, but for a single driver only
	// one will be present.
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ connected_oriented;

	// Stores for which bits we already resolved the orientation (cached in
	// `connected_oriented`).
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> oriented_present;


	ਗਿਣਤੀ class ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ {
		NONE = 0, // Not driven, no need to keep wire
		DRIVEN = 1, // Not driven, uses a value driven elsewhere
		DRIVEN_UNIQUE = 2, // Uses a value driven elsewhere, has at most one direct connection
		KEEP = 3, // Wire that should be kept
		TRISTATE = 4, // Can drive a value but can also use a value driven elsewhere
		DRIVER = 5, // Drives a value
	};

	ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id id_from_drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id id);

	ਖਾਲੀ connect_directed_ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driven_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driver_id);
	ਖਾਲੀ connect_directed_ਬਫਰ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driven_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driver_id);
	ਖਾਲੀ connect_undirected(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id a_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id b_id);

ਜਨਤਕ:

	ਖਾਲੀ ਜੋੜੋ(Module *module);

	// Add a single bit connection to the driver map.
	ਖਾਲੀ ਜੋੜੋ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &a, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &b);

	ਸਾਂਚਾ<typename T>
	ਸਥਿਰ constexpr bool is_sig_type() {
		ਵਾਪਸ
			std::is_same<T, SigSpec>::ਮੁੱਲ ||
			std::is_same<T, SigChunk>::ਮੁੱਲ ||
			std::is_same<T, DriveSpec>::ਮੁੱਲ ||
			std::is_same<T, DriveChunk>::ਮੁੱਲ ||
			std::is_same<T, Drive_Chunk_ਪੋਰਟ>::ਮੁੱਲ ||
			std::is_same<T, DriveChunkWire>::ਮੁੱਲ ||
			std::is_same<T, Const>::ਮੁੱਲ;
	}

	// We use the enable_if to produce better compiler errors when unsupported
	// types are used
	ਸਾਂਚਾ<typename T, typename U>
	typename std::enable_if<is_sig_type<T>() && is_sig_type<U>()>::type
	ਜੋੜੋ(T ਅਚਲ &a, U ਅਚਲ &b)
	{
		log_assert(a.ਮਾਪ() == b.ਮਾਪ());
		ਲਈ (ਪੂਰਨ i = 0; i != Get_ਮਾਪ(a); ++i)
			ਜੋੜੋ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(a[i]), Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(b[i]));
	}


	// Specialized version that avoids unpacking
	ਖਾਲੀ ਜੋੜੋ(SigSpec ਅਚਲ &a, SigSpec ਅਚਲ &b);

ਨਿਜੀ:
	ਖਾਲੀ ਜੋੜੋ_ਪੋਰਟ(Cell *cell, IdString ਪੋਰਟ, SigSpec ਅਚਲ &b);

	// Only used a local variables in `orient_undirected`, always cleared, only
	// stored to reduce allocations.
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> orient_undirected_ਵੇਖਿਆ;
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> orient_undirected_drivers;
	dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, ਪੂਰਨ> orient_undirected_ਦੂਰੀ;

	ਖਾਲੀ orient_undirected(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id id);

ਜਨਤਕ:
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਸੰਚਾਲਕ()(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	DriveSpec ਸੰਚਾਲਕ()(DriveSpec spec);

ਨਿਜੀ:
	bool keep_wire(Wire *wire) {
		// TODO configurable
		ਵਾਪਸ wire->has_attribute(ID(keep));
	}
};

ਸਤਰਅੰਦਰ Hasher Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire::hash_into(Hasher h) ਅਚਲ
{
	h.eat(wire->name);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ::hash_into(Hasher h) ਅਚਲ
{
	h.eat(cell->name);
	h.eat(ਪੋਰਟ);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker::hash_into(Hasher h) ਅਚਲ
{
	h.eat(marker);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ::hash_into(Hasher h) ਅਚਲ
{
	h.eat(ਗੁਣਜ_3);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_ਦੋਆਧਾਰੀ_ਅੰਕ::hash_into(Hasher h) ਅਚਲ
{
	ਚੋਣ (type_) {
	ਹਾਲਤ DriveType::NONE:
		h.eat(0);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::CONSTANT:
		h.eat(constant_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::WIRE:
		h.eat(wire_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::ਪੋਰਟ_2:
		h.eat(ਪੋਰਟ_3);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::MARKER:
		h.eat(marker_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::ਗੁਣਜ:
		h.eat(ਗੁਣਜ_3);
		ਰੋਕੋ;
	}
	h.eat(type_);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher DriveChunkWire::hash_into(Hasher h) ਅਚਲ
{
	h.eat(wire->name);
	h.eat(ਚੌੜਾਈ);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_Chunk_ਪੋਰਟ::hash_into(Hasher h) ਅਚਲ
{
	h.eat(cell->name);
	h.eat(ਪੋਰਟ);
	h.eat(ਚੌੜਾਈ);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher DriveChunkMarker::hash_into(Hasher h) ਅਚਲ
{
	h.eat(marker);
	h.eat(ਚੌੜਾਈ);
	h.eat(ਵਿੱਥ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher Drive_Chunk_ਗੁਣਜ::hash_into(Hasher h) ਅਚਲ
{
	h.eat(ਚੌੜਾਈ_3);
	h.eat(ਗੁਣਜ_3);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher DriveChunk::hash_into(Hasher h) ਅਚਲ
{
	ਚੋਣ (type_) {
	ਹਾਲਤ DriveType::NONE:
		h.eat(0);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::CONSTANT:
		h.eat(constant_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::WIRE:
		h.eat(wire_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::ਪੋਰਟ_2:
		h.eat(ਪੋਰਟ_3);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::MARKER:
		h.eat(marker_);
		ਰੋਕੋ;
	ਹਾਲਤ DriveType::ਗੁਣਜ:
		h.eat(ਗੁਣਜ_3);
		ਰੋਕੋ;
	}
	h.eat(type_);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher DriveSpec::hash_into(Hasher h) ਅਚਲ
{
	updhash();
	h.eat(hash_);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ Hasher DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id::hash_into(Hasher h) ਅਚਲ
{
	h.eat(id);
	ਵਾਪਸ h;
}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
