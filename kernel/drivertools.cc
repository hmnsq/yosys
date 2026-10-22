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

#ಅನ್ತರ್ಭಾವ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

Drive_ಬಿಟ್::Drive_ಬಿಟ್(Sig_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (ಬಿಟ್.is_wire())
		*ಇದು = Drive_ಬಿಟ್_Wire(ಬಿಟ್.wire, ಬಿಟ್.ಸ್ಥಾನಾಂತರ);
	ಇಲ್ಲದಿದ್ದರೆ
		*ಇದು = ಬಿಟ್.ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ Drive_ಬಿಟ್::ವಿಲೀನ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &other)
{
	ಆದರೆ (other.type_ == DriveType::NONE)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (type_ == DriveType::NONE) {
		*ಇದು = other;
		ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (type_ != DriveType::ಅಪವರ್ತ್ಯ) {
		Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ multi(std::move(*ಇದು));
		*ಇದು = std::move(multi);
	}
	ಅಪವರ್ತ್ಯ_2().ವಿಲೀನ(other);
}


ಶೂನ್ಯ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ::ವಿಲೀನ(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single)
{
	ಆದರೆ (single.type() == DriveType::NONE)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (single.type() == DriveType::ಅಪವರ್ತ್ಯ) {
		ವಿಲೀನ(single.ಅಪವರ್ತ್ಯ_2());
		ಹಿಂತಿರುಗಿ;
	}
	ಅಪವರ್ತ್ಯ_3.emplace(single);
}

ಶೂನ್ಯ Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ::ವಿಲೀನ(Drive_ಬಿಟ್ &&single)
{
	ಆದರೆ (single.type() == DriveType::NONE)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (single.type() == DriveType::ಅಪವರ್ತ್ಯ) {
		ವಿಲೀನ(std::move(single.ಅಪವರ್ತ್ಯ_2()));
		ಹಿಂತಿರುಗಿ;
	}
	ಅಪವರ್ತ್ಯ_3.emplace(std::move(single));
}

Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ Drive_Chunk_ಅಪವರ್ತ್ಯ::ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) ಸ್ಥಿರಾಂಕ
{
	Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಫಲಿತಾಂಶ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &single : ಅಪವರ್ತ್ಯ_3)
		ಫಲಿತಾಂಶ.ವಿಲೀನ(single[i]);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


ತಾರ್ಕಿಕ DriveChunkWire::can_append(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಬಿಟ್.wire == wire && ಬಿಟ್.ಸ್ಥಾನಾಂತರ == ಸ್ಥಾನಾಂತರ + ಅಗಲ;
}

ತಾರ್ಕಿಕ DriveChunkWire::try_append(Drive_ಬಿಟ್_Wire ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (!can_append(ಬಿಟ್))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += 1;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ DriveChunkWire::try_append(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (chunk.wire != wire || chunk.ಸ್ಥಾನಾಂತರ != ಸ್ಥಾನಾಂತರ + ಅಗಲ)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += chunk.ಅಗಲ;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಪೋರ್ಟ್::can_append(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಬಿಟ್.cell == cell && ಬಿಟ್.ಪೋರ್ಟ್ == ಪೋರ್ಟ್ && ಬಿಟ್.ಸ್ಥಾನಾಂತರ == ಸ್ಥಾನಾಂತರ + ಅಗಲ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಪೋರ್ಟ್::try_append(Drive_ಬಿಟ್_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (!can_append(ಬಿಟ್))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += 1;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಪೋರ್ಟ್::try_append(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (chunk.cell != cell || chunk.ಪೋರ್ಟ್ != ಪೋರ್ಟ್ || chunk.ಸ್ಥಾನಾಂತರ != ಸ್ಥಾನಾಂತರ + ಅಗಲ)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += chunk.ಅಗಲ;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ DriveChunkMarker::can_append(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಬಿಟ್.marker == marker && ಬಿಟ್.ಸ್ಥಾನಾಂತರ == ಸ್ಥಾನಾಂತರ + ಅಗಲ;
}

ತಾರ್ಕಿಕ DriveChunkMarker::try_append(Drive_ಬಿಟ್_Marker ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (!can_append(ಬಿಟ್))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += 1;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ DriveChunkMarker::try_append(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (chunk.marker != marker || chunk.ಸ್ಥಾನಾಂತರ != ಸ್ಥಾನಾಂತರ + ಅಗಲ)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ += chunk.ಅಗಲ;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}


ತಾರ್ಕಿಕ Drive_Chunk_ಅಪವರ್ತ್ಯ::can_append(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಬಿಟ್.ಅಪವರ್ತ್ಯ_2().ಗಾತ್ರ_2() != ಅಪವರ್ತ್ಯ_3.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಪೂರ್ಣಾಂಕ const_drivers = 0;
	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : ಅಪವರ್ತ್ಯ_3)
		ಆದರೆ (single.is_constant())
			const_drivers += 1;

	ಆದರೆ (const_drivers > 1)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಗಾಗಿ (Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single : ಬಿಟ್.ಅಪವರ್ತ್ಯ_2())
		ಆದರೆ (single.is_constant())
			const_drivers -= 1;

	ಆದರೆ (const_drivers != 0)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : ಅಪವರ್ತ್ಯ_3)
	{
		ಆಯ್ಕೆ (single.type())
		{
			ಸ್ಥಿತಿ DriveType::CONSTANT: {
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &wire = single.wire();
				Drive_ಬಿಟ್ ಮುಂದಿನ = Drive_ಬಿಟ್_Wire(wire.wire, wire.ಸ್ಥಾನಾಂತರ + wire.ಅಗಲ);
				ಆದರೆ (!ಬಿಟ್.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್ = single.ಪೋರ್ಟ್();
				Drive_ಬಿಟ್ ಮುಂದಿನ = Drive_ಬಿಟ್_ಪೋರ್ಟ್(ಪೋರ್ಟ್.cell, ಪೋರ್ಟ್.ಪೋರ್ಟ್, ಪೋರ್ಟ್.ಸ್ಥಾನಾಂತರ + ಪೋರ್ಟ್.ಅಗಲ);
				ಆದರೆ (!ಬಿಟ್.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &marker = single.marker();
				Drive_ಬಿಟ್ ಮುಂದಿನ = Drive_ಬಿಟ್_Marker(marker.marker, marker.ಸ್ಥಾನಾಂತರ + marker.ಅಗಲ);
				ಆದರೆ (!ಬಿಟ್.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಅಪವರ್ತ್ಯ::can_append(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &chunk) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (chunk.ಅಪವರ್ತ್ಯ_2().ಗಾತ್ರ_2() != ಅಪವರ್ತ್ಯ_3.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಪೂರ್ಣಾಂಕ const_drivers = 0;
	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : ಅಪವರ್ತ್ಯ_3)
		ಆದರೆ (single.is_constant())
			const_drivers += 1;

	ಆದರೆ (const_drivers > 1)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : chunk.ಅಪವರ್ತ್ಯ_2())
		ಆದರೆ (single.is_constant())
			const_drivers -= 1;

	ಆದರೆ (const_drivers != 0)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : ಅಪವರ್ತ್ಯ_3)
	{
		ಆಯ್ಕೆ (single.type())
		{
			ಸ್ಥಿತಿ DriveType::CONSTANT: {
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &wire = single.wire();
				DriveChunk ಮುಂದಿನ = DriveChunkWire(wire.wire, wire.ಸ್ಥಾನಾಂತರ + wire.ಅಗಲ, chunk.ಗಾತ್ರ_2());
				ಆದರೆ (!chunk.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್ = single.ಪೋರ್ಟ್();
				DriveChunk ಮುಂದಿನ = Drive_Chunk_ಪೋರ್ಟ್(ಪೋರ್ಟ್.cell, ಪೋರ್ಟ್.ಪೋರ್ಟ್, ಪೋರ್ಟ್.ಸ್ಥಾನಾಂತರ + ಪೋರ್ಟ್.ಅಗಲ, chunk.ಗಾತ್ರ_2());
				ಆದರೆ (!chunk.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER: {
				ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &marker = single.marker();
				DriveChunk ಮುಂದಿನ = DriveChunkMarker(marker.marker, marker.ಸ್ಥಾನಾಂತರ + marker.ಅಗಲ, chunk.ಗಾತ್ರ_2());
				ಆದರೆ (!chunk.ಅಪವರ್ತ್ಯ_2().ಎಣಿಕೆ_2(ಮುಂದಿನ))
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			} ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಅಪವರ್ತ್ಯ::try_append(Drive_ಬಿಟ್_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (!can_append(ಬಿಟ್))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಅಗಲ_3 += 1;
	State constant;

	ಗಾಗಿ (Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &single : ಬಿಟ್.ಅಪವರ್ತ್ಯ_2())
		ಆದರೆ (single.is_constant())
			constant = single.constant();

	ಗಾಗಿ (DriveChunk &single : ಅಪವರ್ತ್ಯ_3)
	{
		ಆಯ್ಕೆ (single.type())
		{
			ಸ್ಥಿತಿ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE: {
				single.wire().ಅಗಲ += 1;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
				single.ಪೋರ್ಟ್().ಅಗಲ += 1;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER: {
				single.marker().ಅಗಲ += 1;
			} ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				log_abort();
		}
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Drive_Chunk_ಅಪವರ್ತ್ಯ::try_append(Drive_Chunk_ಅಪವರ್ತ್ಯ ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (!can_append(chunk))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಪೂರ್ಣಾಂಕ ಅಗಲ = chunk.ಗಾತ್ರ_2();
	ಅಗಲ_3 += ಅಗಲ;
	Const constant;

	ಗಾಗಿ (DriveChunk ಸ್ಥಿರಾಂಕ &single : chunk.ಅಪವರ್ತ್ಯ_2())
		ಆದರೆ (single.is_constant())
			constant = single.constant();

	ಗಾಗಿ (DriveChunk &single : ಅಪವರ್ತ್ಯ_3)
	{
		ಆಯ್ಕೆ (single.type())
		{
			ಸ್ಥಿತಿ DriveType::CONSTANT: {
				single.constant().append(constant);
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::WIRE: {
				single.wire().ಅಗಲ += ಅಗಲ;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
				single.ಪೋರ್ಟ್().ಅಗಲ += ಅಗಲ;
			} ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ DriveType::MARKER: {
				single.marker().ಅಗಲ += ಅಗಲ;
			} ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				log_abort();
		}
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ DriveChunk::can_append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಗಾತ್ರ_2() == 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (ಬಿಟ್.type() != type_)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆಯ್ಕೆ (type_)
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::WIRE:
			ಹಿಂತಿರುಗಿ wire_.can_append(ಬಿಟ್.wire());
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3.can_append(ಬಿಟ್.ಪೋರ್ಟ್());
		ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
			ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3.can_append(ಬಿಟ್.ಅಪವರ್ತ್ಯ_2());
		ಪೂರ್ವನಿಯೋಜಿತ:
			log_abort();
	}
}

ತಾರ್ಕಿಕ DriveChunk::try_append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (ಗಾತ್ರ_2() == 0)
		*ಇದು = ಬಿಟ್;
	ಆದರೆ (ಬಿಟ್.type() != type_)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆಯ್ಕೆ (type_)
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			none_ += 1;
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(ಬಿಟ್.constant()));
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::WIRE:
			ಹಿಂತಿರುಗಿ wire_.try_append(ಬಿಟ್.wire());
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3.try_append(ಬಿಟ್.ಪೋರ್ಟ್());
		ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
			ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3.try_append(ಬಿಟ್.ಅಪವರ್ತ್ಯ_2());
		ಪೂರ್ವನಿಯೋಜಿತ:
			log_abort();
	}
}


ತಾರ್ಕಿಕ DriveChunk::try_append(DriveChunk ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (ಗಾತ್ರ_2() == 0)
		*ಇದು = chunk;
	ಆದರೆ (chunk.type_ != type_)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆಯ್ಕೆ (type_)
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			none_ += chunk.none_;
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಸ್ಥಿತಿ DriveType::WIRE:
			ಹಿಂತಿರುಗಿ wire_.try_append(chunk.wire());
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_3.try_append(chunk.ಪೋರ್ಟ್());
		ಸ್ಥಿತಿ DriveType::MARKER:
			ಹಿಂತಿರುಗಿ marker_.try_append(chunk.marker());
		ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ:
			ಹಿಂತಿರುಗಿ ಅಪವರ್ತ್ಯ_3.try_append(chunk.ಅಪವರ್ತ್ಯ_2());
	}
	log_abort();
}

ಶೂನ್ಯ DriveSpec::append(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	hash_ = 0;
	ಆದರೆ (!packed()) {
		ಬಿಟ್ಟುಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
		ಅಗಲ_3 += 1;
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (chunks_.ಖಾಲಿಗಳು() || !chunks_.ಹಿಂಭಾಗ().try_append(ಬಿಟ್))
		chunks_.emplace_ಹಿಂಭಾಗ(ಬಿಟ್);
	ಅಗಲ_3 += 1;
}

ಶೂನ್ಯ DriveSpec::append(DriveChunk ಸ್ಥಿರಾಂಕ &chunk)
{
	hash_ = 0;
	pack();
	ಆದರೆ (chunks_.ಖಾಲಿಗಳು() || !chunks_.ಹಿಂಭಾಗ().try_append(chunk))
		chunks_.emplace_ಹಿಂಭಾಗ(chunk);
	ಅಗಲ_3 += chunk.ಗಾತ್ರ_2();
}

ಶೂನ್ಯ DriveSpec::pack() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ಬಿಟ್ಟುಗಳು_2.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;
	std::ಸದಿಶ<Drive_ಬಿಟ್> ಬಿಟ್ಟುಗಳು(std::move(ಬಿಟ್ಟುಗಳು_2));
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
		ಆದರೆ (chunks_.ಖಾಲಿಗಳು() || !chunks_.ಹಿಂಭಾಗ().try_append(ಬಿಟ್))
			chunks_.emplace_ಹಿಂಭಾಗ(std::move(ಬಿಟ್));
}

ಶೂನ್ಯ DriveSpec::unpack() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (chunks_.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks_)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ಅಗಲ = chunk.ಗಾತ್ರ_2(); i != ಅಗಲ; ++i)
		{
			ಬಿಟ್ಟುಗಳು_2.emplace_ಹಿಂಭಾಗ(chunk[i]);
		}
	}
	chunks_.clear();
}

ಶೂನ್ಯ DriveSpec::compute_ಅಗಲ()
{
	ಅಗಲ_3 = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &chunk : chunks_)
		ಅಗಲ_3 += chunk.ಗಾತ್ರ_2();
}

ಶೂನ್ಯ DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::ಕೂಡಿಸು_ಅಂಚು(Drive_ಬಿಟ್_Id src, Drive_ಬಿಟ್_Id dst)
{
	ಆದರೆ (ಮೊದಲ_ಅಂಚುಗಳು.emplace(src, dst).ಮೊದಲ->ಎರಡನೇ == dst)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (ಎರಡನೇ_ಅಂಚುಗಳು.emplace(src, dst).ಮೊದಲ->ಎರಡನೇ == dst)
		ಹಿಂತಿರುಗಿ;
	ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು[src].emplace(dst);
}

DriverMap::Drive_ಬಿಟ್_Id DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::ತೆಗೆ_ಅಂಚು(Drive_ಬಿಟ್_Id src)
{
	// TODO unused I think?
	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು_ಇನ್ನಷ್ಟು = ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.ಕಂಡುಹಿಡಿ(src);
	ಆದರೆ (ಸಿಕ್ಕಿತು_ಇನ್ನಷ್ಟು != ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.ಕೊನೆ()) {
		ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = ಸಿಕ್ಕಿತು_ಇನ್ನಷ್ಟು->ಎರಡನೇ.ತೆಗೆ_2();
		ಆದರೆ (ಸಿಕ್ಕಿತು_ಇನ್ನಷ್ಟು->ಎರಡನೇ.ಖಾಲಿಗಳು())
			ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.erase(ಸಿಕ್ಕಿತು_ಇನ್ನಷ್ಟು);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು_ಎರಡನೇ = ಎರಡನೇ_ಅಂಚುಗಳು.ಕಂಡುಹಿಡಿ(src);
	ಆದರೆ (ಸಿಕ್ಕಿತು_ಎರಡನೇ != ಎರಡನೇ_ಅಂಚುಗಳು.ಕೊನೆ()) {
		ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = ಸಿಕ್ಕಿತು_ಎರಡನೇ->ಎರಡನೇ;
		ಎರಡನೇ_ಅಂಚುಗಳು.erase(ಸಿಕ್ಕಿತು_ಎರಡನೇ);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು_ಮೊದಲ = ಮೊದಲ_ಅಂಚುಗಳು.ಕಂಡುಹಿಡಿ(src);
	ಆದರೆ (ಸಿಕ್ಕಿತು_ಮೊದಲ != ಮೊದಲ_ಅಂಚುಗಳು.ಕೊನೆ()) {
		ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = ಸಿಕ್ಕಿತು_ಮೊದಲ->ಎರಡನೇ;
		ಮೊದಲ_ಅಂಚುಗಳು.erase(ಸಿಕ್ಕಿತು_ಮೊದಲ);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಹಿಂತಿರುಗಿ Drive_ಬಿಟ್_Id();
}

ಶೂನ್ಯ DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::clear(Drive_ಬಿಟ್_Id src)
{
	ಮೊದಲ_ಅಂಚುಗಳು.erase(src);
	ಎರಡನೇ_ಅಂಚುಗಳು.erase(src);
	ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.erase(src);
}

ತಾರ್ಕಿಕ DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::ಒಳಗೊಂಡಿದೆ(Drive_ಬಿಟ್_Id src)
{
	ಹಿಂತಿರುಗಿ ಮೊದಲ_ಅಂಚುಗಳು.ಎಣಿಕೆ_2(src);
}

ಪೂರ್ಣಾಂಕ DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::ಎಣಿಕೆ_2(Drive_ಬಿಟ್_Id src)
{
	ಆದರೆ (!ಮೊದಲ_ಅಂಚುಗಳು.ಎಣಿಕೆ_2(src))
		ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (!ಎರಡನೇ_ಅಂಚುಗಳು.ಎಣಿಕೆ_2(src))
		ಹಿಂತಿರುಗಿ 1;
	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.ಕಂಡುಹಿಡಿ(src);
	ಆದರೆ (ಸಿಕ್ಕಿತು == ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ 2;
	ಹಿಂತಿರುಗಿ Get_ಗಾತ್ರ(ಸಿಕ್ಕಿತು->ಎರಡನೇ) + 2;
}

DriverMap::Drive_ಬಿಟ್_Id DriverMap::Drive_ಬಿಟ್_ನಕ್ಷೆ::at(Drive_ಬಿಟ್_Id src, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ (ಸೂಚ್ಯಂಕ == 0)
		ಹಿಂತಿರುಗಿ ಮೊದಲ_ಅಂಚುಗಳು.at(src);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸೂಚ್ಯಂಕ == 1)
		ಹಿಂತಿರುಗಿ ಎರಡನೇ_ಅಂಚುಗಳು.at(src);
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ *ಇನ್ನಷ್ಟು_ಅಂಚುಗಳು.at(src).element(ಸೂಚ್ಯಂಕ - 2);
}


DriverMap::ಬಿಟ್_ಬಹುಲಕ DriverMap::ಬಿಟ್_ಬಹುಲಕ_2(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆಯ್ಕೆ (ಬಿಟ್.type())
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::NONE;
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ಹಿಂತಿರುಗಿ ಬಿಟ್.constant() == State::Sz ? ಬಿಟ್_ಬಹುಲಕ::NONE : ಬಿಟ್_ಬಹುಲಕ::DRIVER;
		ಸ್ಥಿತಿ DriveType::WIRE: {
			ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &wire = ಬಿಟ್.wire();
			ತಾರ್ಕಿಕ driver = wire.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು;
			ತಾರ್ಕಿಕ driven = wire.wire->ಪೋರ್ಟ್_output;

			ಆದರೆ (driver && !driven)
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::DRIVER;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (driven && !driver)
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::DRIVEN;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (driver && driven)
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::TRISTATE;
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ keep_wire(ಬಿಟ್.wire().wire) ? ಬಿಟ್_ಬಹುಲಕ::KEEP : ಬಿಟ್_ಬಹುಲಕ::NONE;
		}
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
			ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್ = ಬಿಟ್.ಪೋರ್ಟ್();
			ತಾರ್ಕಿಕ driver = celltypes.cell_output(ಪೋರ್ಟ್.cell->type, ಪೋರ್ಟ್.ಪೋರ್ಟ್);
			ತಾರ್ಕಿಕ driven = celltypes.cell_ಒಳಹರಿವು(ಪೋರ್ಟ್.cell->type, ಪೋರ್ಟ್.ಪೋರ್ಟ್);
			ಆದರೆ (driver && !driven)
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::DRIVER;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (driven && !driver)
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE;
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ ಬಿಟ್_ಬಹುಲಕ::TRISTATE;
		}
		ಸ್ಥಿತಿ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		ಪೂರ್ವನಿಯೋಜಿತ:
			log_abort();
	}
}

DriverMap::Drive_ಬಿಟ್_Id DriverMap::id_from_drive_ಬಿಟ್(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆಯ್ಕೆ (ಬಿಟ್.type())
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			ಹಿಂತಿರುಗಿ -1;
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಬಿಟ್.constant();
		ಸ್ಥಿತಿ DriveType::WIRE: {
			ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &wire_ಬಿಟ್ = ಬಿಟ್.wire();
			ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
			ಸ್ವಯಂಚಾಲಿತ insertion = wire_offsets.emplace(wire_ಬಿಟ್.wire, ಸ್ಥಾನಾಂತರ);
			ಆದರೆ (insertion.ಎರಡನೇ) {
				ಆದರೆ (wire_ಬಿಟ್.wire->ಅಗಲ == 1) {
					log_assert(wire_ಬಿಟ್.ಸ್ಥಾನಾಂತರ == 0);
					isolated_drive_ಬಿಟ್ಟುಗಳು.emplace(ಸ್ಥಾನಾಂತರ, ಬಿಟ್);
				} ಇಲ್ಲದಿದ್ದರೆ
					drive_ಬಿಟ್ಟುಗಳು.emplace(ಸ್ಥಾನಾಂತರ, Drive_ಬಿಟ್_Wire(wire_ಬಿಟ್.wire, 0));
				ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ += wire_ಬಿಟ್.wire->ಅಗಲ;
			}
			ಹಿಂತಿರುಗಿ insertion.ಮೊದಲ->ಎರಡನೇ.id + wire_ಬಿಟ್.ಸ್ಥಾನಾಂತರ;
		}
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2: {
			ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಪೋರ್ಟ್_ಬಿಟ್ = ಬಿಟ್.ಪೋರ್ಟ್();
			ಸ್ವಯಂಚಾಲಿತ ಕೀಲಿ = std::make_pair(ಪೋರ್ಟ್_ಬಿಟ್.cell, ಪೋರ್ಟ್_ಬಿಟ್.ಪೋರ್ಟ್);
			ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
			ಸ್ವಯಂಚಾಲಿತ insertion = ಪೋರ್ಟ್_offsets.emplace(ಕೀಲಿ, ಸ್ಥಾನಾಂತರ);
			ಆದರೆ (insertion.ಎರಡನೇ) {
				ಪೂರ್ಣಾಂಕ ಅಗಲ = ಪೋರ್ಟ್_ಬಿಟ್.cell->connections().at(ಪೋರ್ಟ್_ಬಿಟ್.ಪೋರ್ಟ್).ಗಾತ್ರ_2();
				ಆದರೆ (ಅಗಲ == 1 && ಸ್ಥಾನಾಂತರ == 0) {
					log_assert(ಪೋರ್ಟ್_ಬಿಟ್.ಸ್ಥಾನಾಂತರ == 0);
					isolated_drive_ಬಿಟ್ಟುಗಳು.emplace(ಸ್ಥಾನಾಂತರ, ಬಿಟ್);
				} ಇಲ್ಲದಿದ್ದರೆ
					drive_ಬಿಟ್ಟುಗಳು.emplace(ಸ್ಥಾನಾಂತರ, Drive_ಬಿಟ್_ಪೋರ್ಟ್(ಪೋರ್ಟ್_ಬಿಟ್.cell, ಪೋರ್ಟ್_ಬಿಟ್.ಪೋರ್ಟ್, 0));
				ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ += ಅಗಲ;
			}
			ಹಿಂತಿರುಗಿ insertion.ಮೊದಲ->ಎರಡನೇ.id + ಪೋರ್ಟ್_ಬಿಟ್.ಸ್ಥಾನಾಂತರ;
		}
		ಪೂರ್ವನಿಯೋಜಿತ:
			log_assert(ಸುಳ್ಳು && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

Drive_ಬಿಟ್ DriverMap::drive_ಬಿಟ್_from_id(Drive_ಬಿಟ್_Id id)
{
	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು_isolated = isolated_drive_ಬಿಟ್ಟುಗಳು.ಕಂಡುಹಿಡಿ(id);
	ಆದರೆ (ಸಿಕ್ಕಿತು_isolated != isolated_drive_ಬಿಟ್ಟುಗಳು.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_isolated->ಎರಡನೇ;

	ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = drive_ಬಿಟ್ಟುಗಳು.ಊರ್ಧ್ವ_bound(id);
	ಆದರೆ (ಸಿಕ್ಕಿತು == drive_ಬಿಟ್ಟುಗಳು.begin()) {
		ಹಿಂತಿರುಗಿ id < 0 ? Drive_ಬಿಟ್() : Drive_ಬಿಟ್((State) id.id);
	}
	--ಸಿಕ್ಕಿತು;
	Drive_ಬಿಟ್ ಫಲಿತಾಂಶ = ಸಿಕ್ಕಿತು->ಎರಡನೇ;
	ಆದರೆ (ಫಲಿತಾಂಶ.is_wire()) {
		ಫಲಿತಾಂಶ.wire().ಸ್ಥಾನಾಂತರ += id.id - ಸಿಕ್ಕಿತು->ಮೊದಲ.id;
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_assert(ಫಲಿತಾಂಶ.is_ಪೋರ್ಟ್());
		ಫಲಿತಾಂಶ.ಪೋರ್ಟ್().ಸ್ಥಾನಾಂತರ += id.id - ಸಿಕ್ಕಿತು->ಮೊದಲ.id;
	}
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ DriverMap::connect_directed_ವಿಲೀನ(Drive_ಬಿಟ್_Id driven_id, Drive_ಬಿಟ್_Id driver_id)
{
	ಆದರೆ (driven_id == driver_id)
		ಹಿಂತಿರುಗಿ;

	same_driver.ವಿಲೀನ(driven_id, driver_id);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ಕೊನೆ = connected_drivers.ಎಣಿಕೆ_2(driven_id); i != ಕೊನೆ; ++i)
		connected_drivers.ಕೂಡಿಸು_ಅಂಚು(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ಕೊನೆ = connected_undirected.ಎಣಿಕೆ_2(driven_id); i != ಕೊನೆ; ++i)
		connected_undirected.ಕೂಡಿಸು_ಅಂಚು(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

ಶೂನ್ಯ DriverMap::connect_directed_ಬಫರ್(Drive_ಬಿಟ್_Id driven_id, Drive_ಬಿಟ್_Id driver_id)
{
	connected_drivers.ಕೂಡಿಸು_ಅಂಚು(driven_id, driver_id);
}

ಶೂನ್ಯ DriverMap::connect_undirected(Drive_ಬಿಟ್_Id a_id, Drive_ಬಿಟ್_Id b_id)
{
	connected_undirected.ಕೂಡಿಸು_ಅಂಚು(a_id, b_id);
	connected_undirected.ಕೂಡಿಸು_ಅಂಚು(b_id, a_id);
}

ಶೂನ್ಯ DriverMap::ಕೂಡಿಸು(Module *module)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &conn : module->connections())
		ಕೂಡಿಸು(conn.ಮೊದಲ, conn.ಎರಡನೇ);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &conn : cell->connections())
			ಕೂಡಿಸು_ಪೋರ್ಟ್(cell, conn.ಮೊದಲ, conn.ಎರಡನೇ);
}

// Add a single bit connection to the driver map.
ಶೂನ್ಯ DriverMap::ಕೂಡಿಸು(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &a, Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &b)
{
	Drive_ಬಿಟ್_Id a_id = id_from_drive_ಬಿಟ್(a);
	Drive_ಬಿಟ್_Id b_id = id_from_drive_ಬಿಟ್(b);

	Drive_ಬಿಟ್_Id orig_a_id = a_id;
	Drive_ಬಿಟ್_Id orig_b_id = b_id;

	a_id = same_driver.ಕಂಡುಹಿಡಿ(a_id);
	b_id = same_driver.ಕಂಡುಹಿಡಿ(b_id);

	ಆದರೆ (a_id == b_id)
		ಹಿಂತಿರುಗಿ;

	ಬಿಟ್_ಬಹುಲಕ a_ಬಹುಲಕ = ಬಿಟ್_ಬಹುಲಕ_2(orig_a_id == a_id ? a : drive_ಬಿಟ್_from_id(a_id));
	ಬಿಟ್_ಬಹುಲಕ b_ಬಹುಲಕ = ಬಿಟ್_ಬಹುಲಕ_2(orig_b_id == b_id ? b : drive_ಬಿಟ್_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ಆದರೆ (a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::NONE && !(b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE || b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN))
		connect_directed_ವಿಲೀನ(a_id, b_id);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::NONE && !(a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE || a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN))
		connect_directed_ವಿಲೀನ(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE && !(b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE || b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN))
		connect_directed_ಬಫರ್(a_id, b_id);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE && !(a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN_UNIQUE || a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVEN))
		connect_directed_ಬಫರ್(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVER)
		connect_directed_ಬಫರ್(b_id, a_id);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b_ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVER)
		connect_directed_ಬಫರ್(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ಇಲ್ಲದಿದ್ದರೆ
		connect_undirected(a_id, b_id);

	ಹಿಂತಿರುಗಿ;
}

// Specialized version that avoids unpacking
ಶೂನ್ಯ DriverMap::ಕೂಡಿಸು(SigSpec ಸ್ಥಿರಾಂಕ &a, SigSpec ಸ್ಥಿರಾಂಕ &b)
{
	log_assert(a.ಗಾತ್ರ_2() == b.ಗಾತ್ರ_2());
	ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &a_chunks = a.chunks();
	ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &b_chunks = b.chunks();

	ಸ್ವಯಂಚಾಲಿತ a_chunk = a_chunks.begin();
	ಸ್ವಯಂಚಾಲಿತ a_ಕೊನೆ = a_chunks.ಕೊನೆ();
	ಪೂರ್ಣಾಂಕ a_ಸ್ಥಾನಾಂತರ = 0;

	ಸ್ವಯಂಚಾಲಿತ b_chunk = b_chunks.begin();
	ಪೂರ್ಣಾಂಕ b_ಸ್ಥಾನಾಂತರ = 0;

	SigChunk tmp_a, tmp_b;
	ಆಗ (a_chunk != a_ಕೊನೆ) {
		ಪೂರ್ಣಾಂಕ a_ಅಗಲ = a_chunk->ಅಗಲ - a_ಸ್ಥಾನಾಂತರ;
		ಆದರೆ (a_ಅಗಲ == 0) {
			a_ಸ್ಥಾನಾಂತರ = 0;
			++a_chunk;
			ಮುಂದುವರಿ;
		}
		ಪೂರ್ಣಾಂಕ b_ಅಗಲ = b_chunk->ಅಗಲ - b_ಸ್ಥಾನಾಂತರ;
		ಆದರೆ (b_ಅಗಲ == 0) {
			b_ಸ್ಥಾನಾಂತರ = 0;
			++b_chunk;
			ಮುಂದುವರಿ;
		}
		ಪೂರ್ಣಾಂಕ ಅಗಲ = std::min(a_ಅಗಲ, b_ಅಗಲ);
		log_assert(ಅಗಲ > 0);

		SigChunk ಸ್ಥಿರಾಂಕ &a_subchunk =
			a_ಸ್ಥಾನಾಂತರ == 0 && a_ಅಗಲ == ಅಗಲ ? *a_chunk : a_chunk->ಹೊರತೆಗೆ(a_ಸ್ಥಾನಾಂತರ, ಅಗಲ);
		SigChunk ಸ್ಥಿರಾಂಕ &b_subchunk =
			b_ಸ್ಥಾನಾಂತರ == 0 && b_ಅಗಲ == ಅಗಲ ? *b_chunk : b_chunk->ಹೊರತೆಗೆ(b_ಸ್ಥಾನಾಂತರ, ಅಗಲ);

		ಕೂಡಿಸು(a_subchunk, b_subchunk);

		a_ಸ್ಥಾನಾಂತರ += ಅಗಲ;
		b_ಸ್ಥಾನಾಂತರ += ಅಗಲ;
	}
}

ಶೂನ್ಯ DriverMap::ಕೂಡಿಸು_ಪೋರ್ಟ್(Cell *cell, IdString ಪೋರ್ಟ್, SigSpec ಸ್ಥಿರಾಂಕ &b)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &chunk : b.chunks()) {
		ಕೂಡಿಸು(chunk, Drive_Chunk_ಪೋರ್ಟ್(cell, ಪೋರ್ಟ್, ಸ್ಥಾನಾಂತರ, chunk.ಅಗಲ));
		ಸ್ಥಾನಾಂತರ += chunk.ಗಾತ್ರ_2();
	}
}

ಶೂನ್ಯ DriverMap::orient_undirected(Drive_ಬಿಟ್_Id id)
{
	pool<Drive_ಬಿಟ್_Id> &ನೋಡಿದ = orient_undirected_ನೋಡಿದ;
	pool<Drive_ಬಿಟ್_Id> &drivers = orient_undirected_drivers;
	dict<Drive_ಬಿಟ್_Id, ಪೂರ್ಣಾಂಕ> &ದೂರ = orient_undirected_ದೂರ;
	ನೋಡಿದ.clear();
	drivers.clear();

	ನೋಡಿದ.emplace(id);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pos = 0; pos < Get_ಗಾತ್ರ(ನೋಡಿದ); ++pos) {
		Drive_ಬಿಟ್_Id current = *ನೋಡಿದ.element(ನೋಡಿದ.ಗಾತ್ರ_2() - 1 - pos);
		Drive_ಬಿಟ್ ಬಿಟ್ = drive_ಬಿಟ್_from_id(current);

		ಬಿಟ್_ಬಹುಲಕ ಬಹುಲಕ = ಬಿಟ್_ಬಹುಲಕ_2(ಬಿಟ್);

		ಆದರೆ (ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVER || ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::TRISTATE)
			drivers.emplace(current);

		ಆದರೆ (connected_drivers.ಒಳಗೊಂಡಿದೆ(current))
			drivers.emplace(current);

		ಪೂರ್ಣಾಂಕ undirected_driver_ಎಣಿಕೆ = connected_undirected.ಎಣಿಕೆ_2(current);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != undirected_driver_ಎಣಿಕೆ; ++i)
			ನೋಡಿದ.emplace(same_driver.ಕಂಡುಹಿಡಿ(connected_undirected.at(current, i)));
	}

	ಆದರೆ (drivers.ಖಾಲಿಗಳು())
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ನೋಡಿದ_id : ನೋಡಿದ)
			drivers.emplace(ನೋಡಿದ_id);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ driver : drivers)
	{
		ದೂರ.clear();
		ದೂರ.emplace(driver, 0);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ pos = 0; pos < Get_ಗಾತ್ರ(ದೂರ); ++pos) {
			ಸ್ವಯಂಚಾಲಿತ current_it = ದೂರ.element(ದೂರ.ಗಾತ್ರ_2() - 1 - pos);

			Drive_ಬಿಟ್_Id current = current_it->ಮೊದಲ;
			ಪೂರ್ಣಾಂಕ undirected_driver_ಎಣಿಕೆ = connected_undirected.ಎಣಿಕೆ_2(current);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != undirected_driver_ಎಣಿಕೆ; ++i)
			{
				Drive_ಬಿಟ್_Id ಮುಂದಿನ = same_driver.ಕಂಡುಹಿಡಿ(connected_undirected.at(current, i));
				ಸ್ವಯಂಚಾಲಿತ emplaced = ದೂರ.emplace(ಮುಂದಿನ, current_it->ಎರಡನೇ + 1);
				ಆದರೆ (emplaced.ಮೊದಲ->ಎರಡನೇ == current_it->ಎರಡನೇ + 1)
					connected_oriented.ಕೂಡಿಸು_ಅಂಚು(ಮುಂದಿನ, current);
			}
		}
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ನೋಡಿದ_id : ನೋಡಿದ)
		oriented_present.emplace(ನೋಡಿದ_id);
}

Drive_ಬಿಟ್ DriverMap::ಕ್ರಿಯಾಕಾರಕ()(Drive_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್)
{
	ಆದರೆ (ಬಿಟ್.type() == DriveType::MARKER || ಬಿಟ್.type() == DriveType::NONE)
		ಹಿಂತಿರುಗಿ ಬಿಟ್;
	ಆದರೆ (ಬಿಟ್.type() == DriveType::ಅಪವರ್ತ್ಯ)
	{
		Drive_ಬಿಟ್ ಫಲಿತಾಂಶ;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &inner : ಬಿಟ್.ಅಪವರ್ತ್ಯ_2().ಅಪವರ್ತ್ಯ_2())
			ಫಲಿತಾಂಶ.ವಿಲೀನ((*ಇದು)(inner));
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	Drive_ಬಿಟ್_Id ಬಿಟ್_id = id_from_drive_ಬಿಟ್(ಬಿಟ್);

	Drive_ಬಿಟ್_Id ಬಿಟ್_repr_id = same_driver.ಕಂಡುಹಿಡಿ(ಬಿಟ್_id);

	Drive_ಬಿಟ್ ಬಿಟ್_repr = drive_ಬಿಟ್_from_id(ಬಿಟ್_repr_id);

	ಬಿಟ್_ಬಹುಲಕ ಬಹುಲಕ = ಬಿಟ್_ಬಹುಲಕ_2(ಬಿಟ್_repr);

	ಆದರೆ (ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::KEEP && ಬಿಟ್_repr_id != ಬಿಟ್_id)
		ಹಿಂತಿರುಗಿ ಬಿಟ್_repr;

	ಪೂರ್ಣಾಂಕ implicit_driver_ಎಣಿಕೆ = connected_drivers.ಎಣಿಕೆ_2(ಬಿಟ್_repr_id);
	ಆದರೆ (connected_undirected.ಒಳಗೊಂಡಿದೆ(ಬಿಟ್_repr_id) && !oriented_present.ಎಣಿಕೆ_2(ಬಿಟ್_repr_id))
		orient_undirected(ಬಿಟ್_repr_id);

	Drive_ಬಿಟ್ driver;

	ಆದರೆ (ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::DRIVER || ಬಹುಲಕ == ಬಿಟ್_ಬಹುಲಕ::TRISTATE)
		driver = ಬಿಟ್_repr;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != implicit_driver_ಎಣಿಕೆ; ++i)
		driver.ವಿಲೀನ(drive_ಬಿಟ್_from_id(connected_drivers.at(ಬಿಟ್_repr_id, i)));

	ಪೂರ್ಣಾಂಕ oriented_driver_ಎಣಿಕೆ = connected_oriented.ಎಣಿಕೆ_2(ಬಿಟ್_repr_id);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != oriented_driver_ಎಣಿಕೆ; ++i)
		driver.ವಿಲೀನ(drive_ಬಿಟ್_from_id(connected_oriented.at(ಬಿಟ್_repr_id, i)));

	ಹಿಂತಿರುಗಿ driver;
}

DriveSpec DriverMap::ಕ್ರಿಯಾಕಾರಕ()(DriveSpec spec)
{
	DriveSpec ಫಲಿತಾಂಶ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, ಅಗಲ = spec.ಗಾತ್ರ_2(); i != ಅಗಲ; ++i)
		ಫಲಿತಾಂಶ.append((*ಇದು)(spec[i]));

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::string log_signal(DriveChunkWire ಸ್ಥಿರಾಂಕ &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ಆದರೆ (chunk.is_ಸಂಪೂರ್ಣ())
		ಹಿಂತಿರುಗಿ id;
	ಆದರೆ (chunk.ಅಗಲ == 1)
		ಹಿಂತಿರುಗಿ stringf("%s [%d]", id, chunk.ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ stringf("%s [%d:%d]", id, chunk.ಸ್ಥಾನಾಂತರ + chunk.ಅಗಲ - 1, chunk.ಸ್ಥಾನಾಂತರ);
}


std::string log_signal(Drive_Chunk_ಪೋರ್ಟ್ ಸ್ಥಿರಾಂಕ &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string ಪೋರ್ಟ್_id = chunk.cell->twines().unescaped_str(chunk.ಪೋರ್ಟ್);
	ಆದರೆ (chunk.is_ಸಂಪೂರ್ಣ())
		ಹಿಂತಿರುಗಿ stringf("%s <%s>", cell_id, ಪೋರ್ಟ್_id);
	ಆದರೆ (chunk.ಅಗಲ == 1)
		ಹಿಂತಿರುಗಿ stringf("%s <%s> [%d]", cell_id, ಪೋರ್ಟ್_id, chunk.ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ stringf("%s <%s> [%d:%d]", cell_id, ಪೋರ್ಟ್_id, chunk.ಸ್ಥಾನಾಂತರ + chunk.ಅಗಲ - 1, chunk.ಸ್ಥಾನಾಂತರ);
}

std::string log_signal(DriveChunkMarker ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆದರೆ (chunk.ಅಗಲ == 1)
		ಹಿಂತಿರುಗಿ stringf("<marker %d> [%d]", chunk.marker, chunk.ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.ಸ್ಥಾನಾಂತರ + chunk.ಅಗಲ - 1, chunk.ಸ್ಥಾನಾಂತರ);
}

std::string log_signal(DriveChunk ಸ್ಥಿರಾಂಕ &chunk)
{
	ಆಯ್ಕೆ (chunk.type())
	{
		ಸ್ಥಿತಿ DriveType::NONE:
			ಹಿಂತಿರುಗಿ stringf("<none x%d>", chunk.ಗಾತ್ರ_2());
		ಸ್ಥಿತಿ DriveType::CONSTANT:
			ಹಿಂತಿರುಗಿ log_const(chunk.constant());
		ಸ್ಥಿತಿ DriveType::WIRE:
			ಹಿಂತಿರುಗಿ log_signal(chunk.wire());
		ಸ್ಥಿತಿ DriveType::ಪೋರ್ಟ್_2:
			ಹಿಂತಿರುಗಿ log_signal(chunk.ಪೋರ್ಟ್());
		ಸ್ಥಿತಿ DriveType::MARKER:
			ಹಿಂತಿರುಗಿ log_signal(chunk.marker());
		ಸ್ಥಿತಿ DriveType::ಅಪವರ್ತ್ಯ: {
			std::string str = "<multiple";
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = " ";
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &single : chunk.ಅಪವರ್ತ್ಯ_2().ಅಪವರ್ತ್ಯ_2()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ಹಿಂತಿರುಗಿ str;
		}
		ಪೂರ್ವನಿಯೋಜಿತ:
			log_abort();
	}
}

std::string log_signal(DriveSpec ಸ್ಥಿರಾಂಕ &spec)
{
	ಸ್ವಯಂಚಾಲಿತ &chunks = spec.chunks();
	ಆದರೆ (chunks.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ "{}";
	ಆದರೆ (chunks.ಗಾತ್ರ_2() == 1)
		ಹಿಂತಿರುಗಿ log_signal(chunks[0]);

	std::string str;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = "{ ";

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = chunks.rbegin(), ಕೊನೆ = chunks.rend(); i != ಕೊನೆ; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ಹಿಂತಿರುಗಿ str;
}

YOSYS_NAMESPACE_ಕೊನೆ
