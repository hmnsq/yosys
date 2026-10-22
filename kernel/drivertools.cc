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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

Drive_ਦੋਆਧਾਰੀ_ਅੰਕ::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.is_wire())
		*ਇਹ = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(ਦੋਆਧਾਰੀ_ਅੰਕ.wire, ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ);
	ਨਹੀਂਤਾਂ
		*ਇਹ = ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ;
}

ਖਾਲੀ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ::ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &other)
{
	ਜੇ (other.type_ == DriveType::NONE)
		ਵਾਪਸ;
	ਜੇ (type_ == DriveType::NONE) {
		*ਇਹ = other;
		ਵਾਪਸ;
	}
	ਜੇ (type_ != DriveType::ਗੁਣਜ) {
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ multi(std::move(*ਇਹ));
		*ਇਹ = std::move(multi);
	}
	ਗੁਣਜ_2().ਮਿਲਾਓ(other);
}


ਖਾਲੀ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ::ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single)
{
	ਜੇ (single.type() == DriveType::NONE)
		ਵਾਪਸ;
	ਜੇ (single.type() == DriveType::ਗੁਣਜ) {
		ਮਿਲਾਓ(single.ਗੁਣਜ_2());
		ਵਾਪਸ;
	}
	ਗੁਣਜ_3.emplace(single);
}

ਖਾਲੀ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ::ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ &&single)
{
	ਜੇ (single.type() == DriveType::NONE)
		ਵਾਪਸ;
	ਜੇ (single.type() == DriveType::ਗੁਣਜ) {
		ਮਿਲਾਓ(std::move(single.ਗੁਣਜ_2()));
		ਵਾਪਸ;
	}
	ਗੁਣਜ_3.emplace(std::move(single));
}

Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ Drive_Chunk_ਗੁਣਜ::ਸੰਚਾਲਕ[](ਪੂਰਨ i) ਅਚਲ
{
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਨਤੀਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &single : ਗੁਣਜ_3)
		ਨਤੀਜਾ.ਮਿਲਾਓ(single[i]);
	ਵਾਪਸ ਨਤੀਜਾ;
}


ਤਾਰਕਿਕ DriveChunkWire::can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ.wire == wire && ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ == ਵਿੱਥ + ਚੌੜਾਈ;
}

ਤਾਰਕਿਕ DriveChunkWire::try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (!can_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += 1;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ DriveChunkWire::try_append(DriveChunkWire ਅਚਲ &chunk)
{
	ਜੇ (chunk.wire != wire || chunk.ਵਿੱਥ != ਵਿੱਥ + ਚੌੜਾਈ)
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += chunk.ਚੌੜਾਈ;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਪੋਰਟ::can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ.cell == cell && ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ == ਪੋਰਟ && ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ == ਵਿੱਥ + ਚੌੜਾਈ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਪੋਰਟ::try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (!can_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += 1;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਪੋਰਟ::try_append(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk)
{
	ਜੇ (chunk.cell != cell || chunk.ਪੋਰਟ != ਪੋਰਟ || chunk.ਵਿੱਥ != ਵਿੱਥ + ਚੌੜਾਈ)
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += chunk.ਚੌੜਾਈ;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ DriveChunkMarker::can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ.marker == marker && ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ == ਵਿੱਥ + ਚੌੜਾਈ;
}

ਤਾਰਕਿਕ DriveChunkMarker::try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (!can_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += 1;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ DriveChunkMarker::try_append(DriveChunkMarker ਅਚਲ &chunk)
{
	ਜੇ (chunk.marker != marker || chunk.ਵਿੱਥ != ਵਿੱਥ + ਚੌੜਾਈ)
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ += chunk.ਚੌੜਾਈ;
	ਵਾਪਸ ਸੱਚ;
}


ਤਾਰਕਿਕ Drive_Chunk_ਗੁਣਜ::can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2().ਮਾਪ() != ਗੁਣਜ_3.ਮਾਪ())
		ਵਾਪਸ ਝੂਠ;

	ਪੂਰਨ const_drivers = 0;
	ਲਈ (DriveChunk ਅਚਲ &single : ਗੁਣਜ_3)
		ਜੇ (single.is_constant())
			const_drivers += 1;

	ਜੇ (const_drivers > 1)
		ਵਾਪਸ ਝੂਠ;

	ਲਈ (Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single : ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2())
		ਜੇ (single.is_constant())
			const_drivers -= 1;

	ਜੇ (const_drivers != 0)
		ਵਾਪਸ ਝੂਠ;

	ਲਈ (DriveChunk ਅਚਲ &single : ਗੁਣਜ_3)
	{
		ਚੋਣ (single.type())
		{
			ਹਾਲਤ DriveType::CONSTANT: {
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &wire = single.wire();
				Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਗਲਾ = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(wire.wire, wire.ਵਿੱਥ + wire.ਚੌੜਾਈ);
				ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &ਪੋਰਟ = single.ਪੋਰਟ();
				Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਗਲਾ = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(ਪੋਰਟ.cell, ਪੋਰਟ.ਪੋਰਟ, ਪੋਰਟ.ਵਿੱਥ + ਪੋਰਟ.ਚੌੜਾਈ);
				ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &marker = single.marker();
				Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਗਲਾ = Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Marker(marker.marker, marker.ਵਿੱਥ + marker.ਚੌੜਾਈ);
				ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਮੂਲ:
				ਵਾਪਸ ਝੂਠ;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਗੁਣਜ::can_append(Drive_Chunk_ਗੁਣਜ ਅਚਲ &chunk) ਅਚਲ
{
	ਜੇ (chunk.ਗੁਣਜ_2().ਮਾਪ() != ਗੁਣਜ_3.ਮਾਪ())
		ਵਾਪਸ ਝੂਠ;

	ਪੂਰਨ const_drivers = 0;
	ਲਈ (DriveChunk ਅਚਲ &single : ਗੁਣਜ_3)
		ਜੇ (single.is_constant())
			const_drivers += 1;

	ਜੇ (const_drivers > 1)
		ਵਾਪਸ ਝੂਠ;

	ਲਈ (DriveChunk ਅਚਲ &single : chunk.ਗੁਣਜ_2())
		ਜੇ (single.is_constant())
			const_drivers -= 1;

	ਜੇ (const_drivers != 0)
		ਵਾਪਸ ਝੂਠ;

	ਲਈ (DriveChunk ਅਚਲ &single : ਗੁਣਜ_3)
	{
		ਚੋਣ (single.type())
		{
			ਹਾਲਤ DriveType::CONSTANT: {
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &wire = single.wire();
				DriveChunk ਅਗਲਾ = DriveChunkWire(wire.wire, wire.ਵਿੱਥ + wire.ਚੌੜਾਈ, chunk.ਮਾਪ());
				ਜੇ (!chunk.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &ਪੋਰਟ = single.ਪੋਰਟ();
				DriveChunk ਅਗਲਾ = Drive_Chunk_ਪੋਰਟ(ਪੋਰਟ.cell, ਪੋਰਟ.ਪੋਰਟ, ਪੋਰਟ.ਵਿੱਥ + ਪੋਰਟ.ਚੌੜਾਈ, chunk.ਮਾਪ());
				ਜੇ (!chunk.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER: {
				ਸਵੈਚਾਲਿਤ ਅਚਲ &marker = single.marker();
				DriveChunk ਅਗਲਾ = DriveChunkMarker(marker.marker, marker.ਵਿੱਥ + marker.ਚੌੜਾਈ, chunk.ਮਾਪ());
				ਜੇ (!chunk.ਗੁਣਜ_2().ਗਿਣਤੀ_2(ਅਗਲਾ))
					ਵਾਪਸ ਝੂਠ;
			} ਰੋਕੋ;
			ਮੂਲ:
				ਵਾਪਸ ਝੂਠ;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਗੁਣਜ::try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗੁਣਜ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (!can_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
		ਵਾਪਸ ਝੂਠ;
	ਚੌੜਾਈ_3 += 1;
	State constant;

	ਲਈ (Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &single : ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2())
		ਜੇ (single.is_constant())
			constant = single.constant();

	ਲਈ (DriveChunk &single : ਗੁਣਜ_3)
	{
		ਚੋਣ (single.type())
		{
			ਹਾਲਤ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE: {
				single.wire().ਚੌੜਾਈ += 1;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
				single.ਪੋਰਟ().ਚੌੜਾਈ += 1;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER: {
				single.marker().ਚੌੜਾਈ += 1;
			} ਰੋਕੋ;
			ਮੂਲ:
				log_abort();
		}
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Drive_Chunk_ਗੁਣਜ::try_append(Drive_Chunk_ਗੁਣਜ ਅਚਲ &chunk)
{
	ਜੇ (!can_append(chunk))
		ਵਾਪਸ ਝੂਠ;
	ਪੂਰਨ ਚੌੜਾਈ = chunk.ਮਾਪ();
	ਚੌੜਾਈ_3 += ਚੌੜਾਈ;
	Const constant;

	ਲਈ (DriveChunk ਅਚਲ &single : chunk.ਗੁਣਜ_2())
		ਜੇ (single.is_constant())
			constant = single.constant();

	ਲਈ (DriveChunk &single : ਗੁਣਜ_3)
	{
		ਚੋਣ (single.type())
		{
			ਹਾਲਤ DriveType::CONSTANT: {
				single.constant().append(constant);
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::WIRE: {
				single.wire().ਚੌੜਾਈ += ਚੌੜਾਈ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
				single.ਪੋਰਟ().ਚੌੜਾਈ += ਚੌੜਾਈ;
			} ਰੋਕੋ;
			ਹਾਲਤ DriveType::MARKER: {
				single.marker().ਚੌੜਾਈ += ਚੌੜਾਈ;
			} ਰੋਕੋ;
			ਮੂਲ:
				log_abort();
		}
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ DriveChunk::can_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
{
	ਜੇ (ਮਾਪ() == 0)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.type() != type_)
		ਵਾਪਸ ਝੂਠ;
	ਚੋਣ (type_)
	{
		ਹਾਲਤ DriveType::NONE:
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::CONSTANT:
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::WIRE:
			ਵਾਪਸ wire_.can_append(ਦੋਆਧਾਰੀ_ਅੰਕ.wire());
		ਹਾਲਤ DriveType::ਪੋਰਟ_2:
			ਵਾਪਸ ਪੋਰਟ_3.can_append(ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ());
		ਹਾਲਤ DriveType::ਗੁਣਜ:
			ਵਾਪਸ ਗੁਣਜ_3.can_append(ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2());
		ਮੂਲ:
			log_abort();
	}
}

ਤਾਰਕਿਕ DriveChunk::try_append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (ਮਾਪ() == 0)
		*ਇਹ = ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.type() != type_)
		ਵਾਪਸ ਝੂਠ;
	ਚੋਣ (type_)
	{
		ਹਾਲਤ DriveType::NONE:
			none_ += 1;
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(ਦੋਆਧਾਰੀ_ਅੰਕ.constant()));
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::WIRE:
			ਵਾਪਸ wire_.try_append(ਦੋਆਧਾਰੀ_ਅੰਕ.wire());
		ਹਾਲਤ DriveType::ਪੋਰਟ_2:
			ਵਾਪਸ ਪੋਰਟ_3.try_append(ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ());
		ਹਾਲਤ DriveType::ਗੁਣਜ:
			ਵਾਪਸ ਗੁਣਜ_3.try_append(ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2());
		ਮੂਲ:
			log_abort();
	}
}


ਤਾਰਕਿਕ DriveChunk::try_append(DriveChunk ਅਚਲ &chunk)
{
	ਜੇ (ਮਾਪ() == 0)
		*ਇਹ = chunk;
	ਜੇ (chunk.type_ != type_)
		ਵਾਪਸ ਝੂਠ;
	ਚੋਣ (type_)
	{
		ਹਾਲਤ DriveType::NONE:
			none_ += chunk.none_;
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ਵਾਪਸ ਸੱਚ;
		ਹਾਲਤ DriveType::WIRE:
			ਵਾਪਸ wire_.try_append(chunk.wire());
		ਹਾਲਤ DriveType::ਪੋਰਟ_2:
			ਵਾਪਸ ਪੋਰਟ_3.try_append(chunk.ਪੋਰਟ());
		ਹਾਲਤ DriveType::MARKER:
			ਵਾਪਸ marker_.try_append(chunk.marker());
		ਹਾਲਤ DriveType::ਗੁਣਜ:
			ਵਾਪਸ ਗੁਣਜ_3.try_append(chunk.ਗੁਣਜ_2());
	}
	log_abort();
}

ਖਾਲੀ DriveSpec::append(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	hash_ = 0;
	ਜੇ (!packed()) {
		ਬਿੱਟ_2.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਚੌੜਾਈ_3 += 1;
		ਵਾਪਸ;
	}

	ਜੇ (chunks_.ਖਾਲੀ_ਥਾਵਾਂ() || !chunks_.ਪਿੱਠ().try_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
		chunks_.emplace_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਚੌੜਾਈ_3 += 1;
}

ਖਾਲੀ DriveSpec::append(DriveChunk ਅਚਲ &chunk)
{
	hash_ = 0;
	pack();
	ਜੇ (chunks_.ਖਾਲੀ_ਥਾਵਾਂ() || !chunks_.ਪਿੱਠ().try_append(chunk))
		chunks_.emplace_ਪਿੱਠ(chunk);
	ਚੌੜਾਈ_3 += chunk.ਮਾਪ();
}

ਖਾਲੀ DriveSpec::pack() ਅਚਲ {
	ਜੇ (ਬਿੱਟ_2.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;
	std::ਸਦਿਸ਼<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ(std::move(ਬਿੱਟ_2));
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
		ਜੇ (chunks_.ਖਾਲੀ_ਥਾਵਾਂ() || !chunks_.ਪਿੱਠ().try_append(ਦੋਆਧਾਰੀ_ਅੰਕ))
			chunks_.emplace_ਪਿੱਠ(std::move(ਦੋਆਧਾਰੀ_ਅੰਕ));
}

ਖਾਲੀ DriveSpec::unpack() ਅਚਲ {
	ਜੇ (chunks_.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks_)
	{
		ਲਈ (ਪੂਰਨ i = 0, ਚੌੜਾਈ = chunk.ਮਾਪ(); i != ਚੌੜਾਈ; ++i)
		{
			ਬਿੱਟ_2.emplace_ਪਿੱਠ(chunk[i]);
		}
	}
	chunks_.clear();
}

ਖਾਲੀ DriveSpec::compute_ਚੌੜਾਈ()
{
	ਚੌੜਾਈ_3 = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &chunk : chunks_)
		ਚੌੜਾਈ_3 += chunk.ਮਾਪ();
}

ਖਾਲੀ DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::ਜੋੜੋ_ਕਿਨਾਰਾ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id dst)
{
	ਜੇ (ਪਹਿਲਾ_ਕਿਨਾਰੇ.emplace(src, dst).ਪਹਿਲਾ->ਦੂਜਾ == dst)
		ਵਾਪਸ;
	ਜੇ (ਦੂਜਾ_ਕਿਨਾਰੇ.emplace(src, dst).ਪਹਿਲਾ->ਦੂਜਾ == dst)
		ਵਾਪਸ;
	ਹੋਰ_ਕਿਨਾਰੇ[src].emplace(dst);
}

DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::ਕੱਢੋ_ਕਿਨਾਰਾ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src)
{
	// TODO unused I think?
	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ_ਹੋਰ = ਹੋਰ_ਕਿਨਾਰੇ.ਲੱਭੋ(src);
	ਜੇ (ਮਿਲਿਆ_ਹੋਰ != ਹੋਰ_ਕਿਨਾਰੇ.ਸਿਰਾ()) {
		ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = ਮਿਲਿਆ_ਹੋਰ->ਦੂਜਾ.ਕੱਢੋ();
		ਜੇ (ਮਿਲਿਆ_ਹੋਰ->ਦੂਜਾ.ਖਾਲੀ_ਥਾਵਾਂ())
			ਹੋਰ_ਕਿਨਾਰੇ.erase(ਮਿਲਿਆ_ਹੋਰ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ_ਦੂਜਾ = ਦੂਜਾ_ਕਿਨਾਰੇ.ਲੱਭੋ(src);
	ਜੇ (ਮਿਲਿਆ_ਦੂਜਾ != ਦੂਜਾ_ਕਿਨਾਰੇ.ਸਿਰਾ()) {
		ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = ਮਿਲਿਆ_ਦੂਜਾ->ਦੂਜਾ;
		ਦੂਜਾ_ਕਿਨਾਰੇ.erase(ਮਿਲਿਆ_ਦੂਜਾ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ_ਪਹਿਲਾ = ਪਹਿਲਾ_ਕਿਨਾਰੇ.ਲੱਭੋ(src);
	ਜੇ (ਮਿਲਿਆ_ਪਹਿਲਾ != ਪਹਿਲਾ_ਕਿਨਾਰੇ.ਸਿਰਾ()) {
		ਸਵੈਚਾਲਿਤ ਨਤੀਜਾ = ਮਿਲਿਆ_ਪਹਿਲਾ->ਦੂਜਾ;
		ਪਹਿਲਾ_ਕਿਨਾਰੇ.erase(ਮਿਲਿਆ_ਪਹਿਲਾ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਵਾਪਸ Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id();
}

ਖਾਲੀ DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::clear(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src)
{
	ਪਹਿਲਾ_ਕਿਨਾਰੇ.erase(src);
	ਦੂਜਾ_ਕਿਨਾਰੇ.erase(src);
	ਹੋਰ_ਕਿਨਾਰੇ.erase(src);
}

ਤਾਰਕਿਕ DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::ਸ਼ਾਮਲ_ਹੈ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src)
{
	ਵਾਪਸ ਪਹਿਲਾ_ਕਿਨਾਰੇ.ਗਿਣਤੀ_2(src);
}

ਪੂਰਨ DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::ਗਿਣਤੀ_2(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src)
{
	ਜੇ (!ਪਹਿਲਾ_ਕਿਨਾਰੇ.ਗਿਣਤੀ_2(src))
		ਵਾਪਸ 0;
	ਜੇ (!ਦੂਜਾ_ਕਿਨਾਰੇ.ਗਿਣਤੀ_2(src))
		ਵਾਪਸ 1;
	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਹੋਰ_ਕਿਨਾਰੇ.ਲੱਭੋ(src);
	ਜੇ (ਮਿਲਿਆ == ਹੋਰ_ਕਿਨਾਰੇ.ਸਿਰਾ())
		ਵਾਪਸ 2;
	ਵਾਪਸ Get_ਮਾਪ(ਮਿਲਿਆ->ਦੂਜਾ) + 2;
}

DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗ੍ਰਾਫ::at(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id src, ਪੂਰਨ ਸੂਚਕਾਂਕ)
{
	ਜੇ (ਸੂਚਕਾਂਕ == 0)
		ਵਾਪਸ ਪਹਿਲਾ_ਕਿਨਾਰੇ.at(src);
	ਨਹੀਂਤਾਂ ਜੇ (ਸੂਚਕਾਂਕ == 1)
		ਵਾਪਸ ਦੂਜਾ_ਕਿਨਾਰੇ.at(src);
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ *ਹੋਰ_ਕਿਨਾਰੇ.at(src).element(ਸੂਚਕਾਂਕ - 2);
}


DriverMap::ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ DriverMap::ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ.type())
	{
		ਹਾਲਤ DriveType::NONE:
			ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::NONE;
		ਹਾਲਤ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ.constant() == State::Sz ? ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::NONE : ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER;
		ਹਾਲਤ DriveType::WIRE: {
			ਸਵੈਚਾਲਿਤ ਅਚਲ &wire = ਦੋਆਧਾਰੀ_ਅੰਕ.wire();
			ਤਾਰਕਿਕ driver = wire.wire->ਪੋਰਟ_ਨਿਵੇਸ਼;
			ਤਾਰਕਿਕ driven = wire.wire->ਪੋਰਟ_output;

			ਜੇ (driver && !driven)
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER;
			ਨਹੀਂਤਾਂ ਜੇ (driven && !driver)
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN;
			ਨਹੀਂਤਾਂ ਜੇ (driver && driven)
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::TRISTATE;
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ keep_wire(ਦੋਆਧਾਰੀ_ਅੰਕ.wire().wire) ? ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::KEEP : ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::NONE;
		}
		ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
			ਸਵੈਚਾਲਿਤ ਅਚਲ &ਪੋਰਟ = ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ();
			ਤਾਰਕਿਕ driver = celltypes.cell_output(ਪੋਰਟ.cell->type, ਪੋਰਟ.ਪੋਰਟ);
			ਤਾਰਕਿਕ driven = celltypes.cell_ਨਿਵੇਸ਼(ਪੋਰਟ.cell->type, ਪੋਰਟ.ਪੋਰਟ);
			ਜੇ (driver && !driven)
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER;
			ਨਹੀਂਤਾਂ ਜੇ (driven && !driver)
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE;
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::TRISTATE;
		}
		ਹਾਲਤ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		ਮੂਲ:
			log_abort();
	}
}

DriverMap::Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id DriverMap::id_from_drive_ਦੋਆਧਾਰੀ_ਅੰਕ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਚੋਣ (ਦੋਆਧਾਰੀ_ਅੰਕ.type())
	{
		ਹਾਲਤ DriveType::NONE:
			ਵਾਪਸ -1;
		ਹਾਲਤ DriveType::CONSTANT:
			ਵਾਪਸ (ਪੂਰਨ)ਦੋਆਧਾਰੀ_ਅੰਕ.constant();
		ਹਾਲਤ DriveType::WIRE: {
			ਸਵੈਚਾਲਿਤ ਅਚਲ &wire_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ.wire();
			ਪੂਰਨ ਵਿੱਥ = ਅਗਲਾ_ਵਿੱਥ;
			ਸਵੈਚਾਲਿਤ insertion = wire_offsets.emplace(wire_ਦੋਆਧਾਰੀ_ਅੰਕ.wire, ਵਿੱਥ);
			ਜੇ (insertion.ਦੂਜਾ) {
				ਜੇ (wire_ਦੋਆਧਾਰੀ_ਅੰਕ.wire->ਚੌੜਾਈ == 1) {
					log_assert(wire_ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ == 0);
					isolated_drive_ਬਿੱਟ.emplace(ਵਿੱਥ, ਦੋਆਧਾਰੀ_ਅੰਕ);
				} ਨਹੀਂਤਾਂ
					drive_ਬਿੱਟ.emplace(ਵਿੱਥ, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Wire(wire_ਦੋਆਧਾਰੀ_ਅੰਕ.wire, 0));
				ਅਗਲਾ_ਵਿੱਥ += wire_ਦੋਆਧਾਰੀ_ਅੰਕ.wire->ਚੌੜਾਈ;
			}
			ਵਾਪਸ insertion.ਪਹਿਲਾ->ਦੂਜਾ.id + wire_ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ;
		}
		ਹਾਲਤ DriveType::ਪੋਰਟ_2: {
			ਸਵੈਚਾਲਿਤ ਅਚਲ &ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ = ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ();
			ਸਵੈਚਾਲਿਤ ਕੁੰਜੀ = std::make_pair(ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.cell, ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ);
			ਪੂਰਨ ਵਿੱਥ = ਅਗਲਾ_ਵਿੱਥ;
			ਸਵੈਚਾਲਿਤ insertion = ਪੋਰਟ_offsets.emplace(ਕੁੰਜੀ, ਵਿੱਥ);
			ਜੇ (insertion.ਦੂਜਾ) {
				ਪੂਰਨ ਚੌੜਾਈ = ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.cell->connections().at(ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ).ਮਾਪ();
				ਜੇ (ਚੌੜਾਈ == 1 && ਵਿੱਥ == 0) {
					log_assert(ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ == 0);
					isolated_drive_ਬਿੱਟ.emplace(ਵਿੱਥ, ਦੋਆਧਾਰੀ_ਅੰਕ);
				} ਨਹੀਂਤਾਂ
					drive_ਬਿੱਟ.emplace(ਵਿੱਥ, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_ਪੋਰਟ(ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.cell, ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.ਪੋਰਟ, 0));
				ਅਗਲਾ_ਵਿੱਥ += ਚੌੜਾਈ;
			}
			ਵਾਪਸ insertion.ਪਹਿਲਾ->ਦੂਜਾ.id + ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ;
		}
		ਮੂਲ:
			log_assert(ਝੂਠ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_ਦੋਆਧਾਰੀ_ਅੰਕ DriverMap::drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id id)
{
	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ_isolated = isolated_drive_ਬਿੱਟ.ਲੱਭੋ(id);
	ਜੇ (ਮਿਲਿਆ_isolated != isolated_drive_ਬਿੱਟ.ਸਿਰਾ())
		ਵਾਪਸ ਮਿਲਿਆ_isolated->ਦੂਜਾ;

	ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = drive_ਬਿੱਟ.ਉੱਤਲਾ_bound(id);
	ਜੇ (ਮਿਲਿਆ == drive_ਬਿੱਟ.begin()) {
		ਵਾਪਸ id < 0 ? Drive_ਦੋਆਧਾਰੀ_ਅੰਕ() : Drive_ਦੋਆਧਾਰੀ_ਅੰਕ((State) id.id);
	}
	--ਮਿਲਿਆ;
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਨਤੀਜਾ = ਮਿਲਿਆ->ਦੂਜਾ;
	ਜੇ (ਨਤੀਜਾ.is_wire()) {
		ਨਤੀਜਾ.wire().ਵਿੱਥ += id.id - ਮਿਲਿਆ->ਪਹਿਲਾ.id;
	} ਨਹੀਂਤਾਂ {
		log_assert(ਨਤੀਜਾ.is_ਪੋਰਟ());
		ਨਤੀਜਾ.ਪੋਰਟ().ਵਿੱਥ += id.id - ਮਿਲਿਆ->ਪਹਿਲਾ.id;
	}
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ DriverMap::connect_directed_ਮਿਲਾਓ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driven_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driver_id)
{
	ਜੇ (driven_id == driver_id)
		ਵਾਪਸ;

	same_driver.ਮਿਲਾਓ(driven_id, driver_id);

	ਲਈ (ਪੂਰਨ i = 0, ਸਿਰਾ = connected_drivers.ਗਿਣਤੀ_2(driven_id); i != ਸਿਰਾ; ++i)
		connected_drivers.ਜੋੜੋ_ਕਿਨਾਰਾ(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ਲਈ (ਪੂਰਨ i = 0, ਸਿਰਾ = connected_undirected.ਗਿਣਤੀ_2(driven_id); i != ਸਿਰਾ; ++i)
		connected_undirected.ਜੋੜੋ_ਕਿਨਾਰਾ(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

ਖਾਲੀ DriverMap::connect_directed_ਬਫਰ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driven_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id driver_id)
{
	connected_drivers.ਜੋੜੋ_ਕਿਨਾਰਾ(driven_id, driver_id);
}

ਖਾਲੀ DriverMap::connect_undirected(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id a_id, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id b_id)
{
	connected_undirected.ਜੋੜੋ_ਕਿਨਾਰਾ(a_id, b_id);
	connected_undirected.ਜੋੜੋ_ਕਿਨਾਰਾ(b_id, a_id);
}

ਖਾਲੀ DriverMap::ਜੋੜੋ(Module *module)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &conn : module->connections())
		ਜੋੜੋ(conn.ਪਹਿਲਾ, conn.ਦੂਜਾ);

	ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &conn : cell->connections())
			ਜੋੜੋ_ਪੋਰਟ(cell, conn.ਪਹਿਲਾ, conn.ਦੂਜਾ);
}

// Add a single bit connection to the driver map.
ਖਾਲੀ DriverMap::ਜੋੜੋ(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &a, Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &b)
{
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id a_id = id_from_drive_ਦੋਆਧਾਰੀ_ਅੰਕ(a);
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id b_id = id_from_drive_ਦੋਆਧਾਰੀ_ਅੰਕ(b);

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id orig_a_id = a_id;
	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id orig_b_id = b_id;

	a_id = same_driver.ਲੱਭੋ(a_id);
	b_id = same_driver.ਲੱਭੋ(b_id);

	ਜੇ (a_id == b_id)
		ਵਾਪਸ;

	ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ a_ਬਹੁਲਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(orig_a_id == a_id ? a : drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(a_id));
	ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ b_ਬਹੁਲਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(orig_b_id == b_id ? b : drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ਜੇ (a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::NONE && !(b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE || b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN))
		connect_directed_ਮਿਲਾਓ(a_id, b_id);
	ਨਹੀਂਤਾਂ ਜੇ (b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::NONE && !(a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE || a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN))
		connect_directed_ਮਿਲਾਓ(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ਨਹੀਂਤਾਂ ਜੇ (a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE && !(b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE || b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN))
		connect_directed_ਬਫਰ(a_id, b_id);
	ਨਹੀਂਤਾਂ ਜੇ (b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE && !(a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN_UNIQUE || a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVEN))
		connect_directed_ਬਫਰ(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ਨਹੀਂਤਾਂ ਜੇ (a_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER)
		connect_directed_ਬਫਰ(b_id, a_id);
	ਨਹੀਂਤਾਂ ਜੇ (b_ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER)
		connect_directed_ਬਫਰ(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ਨਹੀਂਤਾਂ
		connect_undirected(a_id, b_id);

	ਵਾਪਸ;
}

// Specialized version that avoids unpacking
ਖਾਲੀ DriverMap::ਜੋੜੋ(SigSpec ਅਚਲ &a, SigSpec ਅਚਲ &b)
{
	log_assert(a.ਮਾਪ() == b.ਮਾਪ());
	ਸਵੈਚਾਲਿਤ ਅਚਲ &a_chunks = a.chunks();
	ਸਵੈਚਾਲਿਤ ਅਚਲ &b_chunks = b.chunks();

	ਸਵੈਚਾਲਿਤ a_chunk = a_chunks.begin();
	ਸਵੈਚਾਲਿਤ a_ਸਿਰਾ = a_chunks.ਸਿਰਾ();
	ਪੂਰਨ a_ਵਿੱਥ = 0;

	ਸਵੈਚਾਲਿਤ b_chunk = b_chunks.begin();
	ਪੂਰਨ b_ਵਿੱਥ = 0;

	SigChunk tmp_a, tmp_b;
	ਜਦੋਂ (a_chunk != a_ਸਿਰਾ) {
		ਪੂਰਨ a_ਚੌੜਾਈ = a_chunk->ਚੌੜਾਈ - a_ਵਿੱਥ;
		ਜੇ (a_ਚੌੜਾਈ == 0) {
			a_ਵਿੱਥ = 0;
			++a_chunk;
			ਜਾਰੀ;
		}
		ਪੂਰਨ b_ਚੌੜਾਈ = b_chunk->ਚੌੜਾਈ - b_ਵਿੱਥ;
		ਜੇ (b_ਚੌੜਾਈ == 0) {
			b_ਵਿੱਥ = 0;
			++b_chunk;
			ਜਾਰੀ;
		}
		ਪੂਰਨ ਚੌੜਾਈ = std::min(a_ਚੌੜਾਈ, b_ਚੌੜਾਈ);
		log_assert(ਚੌੜਾਈ > 0);

		SigChunk ਅਚਲ &a_subchunk =
			a_ਵਿੱਥ == 0 && a_ਚੌੜਾਈ == ਚੌੜਾਈ ? *a_chunk : a_chunk->ਕੱਢੋ_2(a_ਵਿੱਥ, ਚੌੜਾਈ);
		SigChunk ਅਚਲ &b_subchunk =
			b_ਵਿੱਥ == 0 && b_ਚੌੜਾਈ == ਚੌੜਾਈ ? *b_chunk : b_chunk->ਕੱਢੋ_2(b_ਵਿੱਥ, ਚੌੜਾਈ);

		ਜੋੜੋ(a_subchunk, b_subchunk);

		a_ਵਿੱਥ += ਚੌੜਾਈ;
		b_ਵਿੱਥ += ਚੌੜਾਈ;
	}
}

ਖਾਲੀ DriverMap::ਜੋੜੋ_ਪੋਰਟ(Cell *cell, IdString ਪੋਰਟ, SigSpec ਅਚਲ &b)
{
	ਪੂਰਨ ਵਿੱਥ = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &chunk : b.chunks()) {
		ਜੋੜੋ(chunk, Drive_Chunk_ਪੋਰਟ(cell, ਪੋਰਟ, ਵਿੱਥ, chunk.ਚੌੜਾਈ));
		ਵਿੱਥ += chunk.ਮਾਪ();
	}
}

ਖਾਲੀ DriverMap::orient_undirected(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id id)
{
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> &ਵੇਖਿਆ = orient_undirected_ਵੇਖਿਆ;
	pool<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id> &drivers = orient_undirected_drivers;
	dict<Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id, ਪੂਰਨ> &ਦੂਰੀ = orient_undirected_ਦੂਰੀ;
	ਵੇਖਿਆ.clear();
	drivers.clear();

	ਵੇਖਿਆ.emplace(id);

	ਲਈ (ਪੂਰਨ pos = 0; pos < Get_ਮਾਪ(ਵੇਖਿਆ); ++pos) {
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id current = *ਵੇਖਿਆ.element(ਵੇਖਿਆ.ਮਾਪ() - 1 - pos);
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(current);

		ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ ਬਹੁਲਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(ਦੋਆਧਾਰੀ_ਅੰਕ);

		ਜੇ (ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER || ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::TRISTATE)
			drivers.emplace(current);

		ਜੇ (connected_drivers.ਸ਼ਾਮਲ_ਹੈ(current))
			drivers.emplace(current);

		ਪੂਰਨ undirected_driver_ਗਿਣਤੀ = connected_undirected.ਗਿਣਤੀ_2(current);

		ਲਈ (ਪੂਰਨ i = 0; i != undirected_driver_ਗਿਣਤੀ; ++i)
			ਵੇਖਿਆ.emplace(same_driver.ਲੱਭੋ(connected_undirected.at(current, i)));
	}

	ਜੇ (drivers.ਖਾਲੀ_ਥਾਵਾਂ())
		ਲਈ (ਸਵੈਚਾਲਿਤ ਵੇਖਿਆ_id : ਵੇਖਿਆ)
			drivers.emplace(ਵੇਖਿਆ_id);

	ਲਈ (ਸਵੈਚਾਲਿਤ driver : drivers)
	{
		ਦੂਰੀ.clear();
		ਦੂਰੀ.emplace(driver, 0);

		ਲਈ (ਪੂਰਨ pos = 0; pos < Get_ਮਾਪ(ਦੂਰੀ); ++pos) {
			ਸਵੈਚਾਲਿਤ current_it = ਦੂਰੀ.element(ਦੂਰੀ.ਮਾਪ() - 1 - pos);

			Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id current = current_it->ਪਹਿਲਾ;
			ਪੂਰਨ undirected_driver_ਗਿਣਤੀ = connected_undirected.ਗਿਣਤੀ_2(current);

			ਲਈ (ਪੂਰਨ i = 0; i != undirected_driver_ਗਿਣਤੀ; ++i)
			{
				Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id ਅਗਲਾ = same_driver.ਲੱਭੋ(connected_undirected.at(current, i));
				ਸਵੈਚਾਲਿਤ emplaced = ਦੂਰੀ.emplace(ਅਗਲਾ, current_it->ਦੂਜਾ + 1);
				ਜੇ (emplaced.ਪਹਿਲਾ->ਦੂਜਾ == current_it->ਦੂਜਾ + 1)
					connected_oriented.ਜੋੜੋ_ਕਿਨਾਰਾ(ਅਗਲਾ, current);
			}
		}
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ ਵੇਖਿਆ_id : ਵੇਖਿਆ)
		oriented_present.emplace(ਵੇਖਿਆ_id);
}

Drive_ਦੋਆਧਾਰੀ_ਅੰਕ DriverMap::ਸੰਚਾਲਕ()(Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.type() == DriveType::MARKER || ਦੋਆਧਾਰੀ_ਅੰਕ.type() == DriveType::NONE)
		ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.type() == DriveType::ਗੁਣਜ)
	{
		Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਨਤੀਜਾ;
		ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &inner : ਦੋਆਧਾਰੀ_ਅੰਕ.ਗੁਣਜ_2().ਗੁਣਜ_2())
			ਨਤੀਜਾ.ਮਿਲਾਓ((*ਇਹ)(inner));
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id ਦੋਆਧਾਰੀ_ਅੰਕ_id = id_from_drive_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ);

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ_Id ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id = same_driver.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ_id);

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ_repr = drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id);

	ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ ਬਹੁਲਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ_2(ਦੋਆਧਾਰੀ_ਅੰਕ_repr);

	ਜੇ (ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::KEEP && ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id != ਦੋਆਧਾਰੀ_ਅੰਕ_id)
		ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_repr;

	ਪੂਰਨ implicit_driver_ਗਿਣਤੀ = connected_drivers.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id);
	ਜੇ (connected_undirected.ਸ਼ਾਮਲ_ਹੈ(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id) && !oriented_present.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id))
		orient_undirected(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id);

	Drive_ਦੋਆਧਾਰੀ_ਅੰਕ driver;

	ਜੇ (ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::DRIVER || ਬਹੁਲਕ == ਦੋਆਧਾਰੀ_ਅੰਕ_ਬਹੁਲਕ::TRISTATE)
		driver = ਦੋਆਧਾਰੀ_ਅੰਕ_repr;

	ਲਈ (ਪੂਰਨ i = 0; i != implicit_driver_ਗਿਣਤੀ; ++i)
		driver.ਮਿਲਾਓ(drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(connected_drivers.at(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id, i)));

	ਪੂਰਨ oriented_driver_ਗਿਣਤੀ = connected_oriented.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id);
	ਲਈ (ਪੂਰਨ i = 0; i != oriented_driver_ਗਿਣਤੀ; ++i)
		driver.ਮਿਲਾਓ(drive_ਦੋਆਧਾਰੀ_ਅੰਕ_from_id(connected_oriented.at(ਦੋਆਧਾਰੀ_ਅੰਕ_repr_id, i)));

	ਵਾਪਸ driver;
}

DriveSpec DriverMap::ਸੰਚਾਲਕ()(DriveSpec spec)
{
	DriveSpec ਨਤੀਜਾ;

	ਲਈ (ਪੂਰਨ i = 0, ਚੌੜਾਈ = spec.ਮਾਪ(); i != ਚੌੜਾਈ; ++i)
		ਨਤੀਜਾ.append((*ਇਹ)(spec[i]));

	ਵਾਪਸ ਨਤੀਜਾ;
}

std::string log_signal(DriveChunkWire ਅਚਲ &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ਜੇ (chunk.is_ਸਾਰਾ())
		ਵਾਪਸ id;
	ਜੇ (chunk.ਚੌੜਾਈ == 1)
		ਵਾਪਸ stringf("%s [%d]", id, chunk.ਵਿੱਥ);
	ਵਾਪਸ stringf("%s [%d:%d]", id, chunk.ਵਿੱਥ + chunk.ਚੌੜਾਈ - 1, chunk.ਵਿੱਥ);
}


std::string log_signal(Drive_Chunk_ਪੋਰਟ ਅਚਲ &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string ਪੋਰਟ_id = chunk.cell->twines().unescaped_str(chunk.ਪੋਰਟ);
	ਜੇ (chunk.is_ਸਾਰਾ())
		ਵਾਪਸ stringf("%s <%s>", cell_id, ਪੋਰਟ_id);
	ਜੇ (chunk.ਚੌੜਾਈ == 1)
		ਵਾਪਸ stringf("%s <%s> [%d]", cell_id, ਪੋਰਟ_id, chunk.ਵਿੱਥ);
	ਵਾਪਸ stringf("%s <%s> [%d:%d]", cell_id, ਪੋਰਟ_id, chunk.ਵਿੱਥ + chunk.ਚੌੜਾਈ - 1, chunk.ਵਿੱਥ);
}

std::string log_signal(DriveChunkMarker ਅਚਲ &chunk)
{
	ਜੇ (chunk.ਚੌੜਾਈ == 1)
		ਵਾਪਸ stringf("<marker %d> [%d]", chunk.marker, chunk.ਵਿੱਥ);
	ਵਾਪਸ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.ਵਿੱਥ + chunk.ਚੌੜਾਈ - 1, chunk.ਵਿੱਥ);
}

std::string log_signal(DriveChunk ਅਚਲ &chunk)
{
	ਚੋਣ (chunk.type())
	{
		ਹਾਲਤ DriveType::NONE:
			ਵਾਪਸ stringf("<none x%d>", chunk.ਮਾਪ());
		ਹਾਲਤ DriveType::CONSTANT:
			ਵਾਪਸ log_const(chunk.constant());
		ਹਾਲਤ DriveType::WIRE:
			ਵਾਪਸ log_signal(chunk.wire());
		ਹਾਲਤ DriveType::ਪੋਰਟ_2:
			ਵਾਪਸ log_signal(chunk.ਪੋਰਟ());
		ਹਾਲਤ DriveType::MARKER:
			ਵਾਪਸ log_signal(chunk.marker());
		ਹਾਲਤ DriveType::ਗੁਣਜ: {
			std::string str = "<multiple";
			ਅਚਲ ਅੱਖਰ *sep = " ";
			ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &single : chunk.ਗੁਣਜ_2().ਗੁਣਜ_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ਵਾਪਸ str;
		}
		ਮੂਲ:
			log_abort();
	}
}

std::string log_signal(DriveSpec ਅਚਲ &spec)
{
	ਸਵੈਚਾਲਿਤ &chunks = spec.chunks();
	ਜੇ (chunks.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ "{}";
	ਜੇ (chunks.ਮਾਪ() == 1)
		ਵਾਪਸ log_signal(chunks[0]);

	std::string str;
	ਅਚਲ ਅੱਖਰ *sep = "{ ";

	ਲਈ (ਸਵੈਚਾਲਿਤ i = chunks.rbegin(), ਸਿਰਾ = chunks.rend(); i != ਸਿਰਾ; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ਵਾਪਸ str;
}

YOSYS_NAMESPACE_ਸਿਰਾ
