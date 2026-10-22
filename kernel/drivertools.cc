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

DriveBit::DriveBit(SigBit ಧ್ರುವ &bit)
{
	ಯದಿ (bit.is_wire())
		*ಏತತ್ = DriveBitWire(bit.wire, bit.offset);
	ಅನ್ಯಥಾ
		*ಏತತ್ = bit.data;
}

ಶೂನ್ಯ DriveBit::merge(DriveBit ಧ್ರುವ &other)
{
	ಯದಿ (other.type_ == DriveType::NONE)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (type_ == DriveType::NONE) {
		*ಏತತ್ = other;
		ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*ಏತತ್));
		*ಏತತ್ = std::move(multi);
	}
	multiple().merge(other);
}


ಶೂನ್ಯ DriveBitMultiple::merge(DriveBit ಧ್ರುವ &single)
{
	ಯದಿ (single.type() == DriveType::NONE)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ಪ್ರತ್ಯಾಗಮ;
	}
	multiple_.emplace(single);
}

ಶೂನ್ಯ DriveBitMultiple::merge(DriveBit &&single)
{
	ಯದಿ (single.type() == DriveType::NONE)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ಪ್ರತ್ಯಾಗಮ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ಪೂರ್ಣಾಙ್ಕ i) ಧ್ರುವ
{
	DriveBitMultiple result;
	ಪ್ರತಿ (auto ಧ್ರುವ &single : multiple_)
		result.merge(single[i]);
	ಪ್ರತ್ಯಾಗಮ result;
}


ತಾರ್ಕಿಕ DriveChunkWire::can_append(DriveBitWire ಧ್ರುವ &bit) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ bit.wire == wire && bit.offset == offset + width;
}

ತಾರ್ಕಿಕ DriveChunkWire::try_append(DriveBitWire ಧ್ರುವ &bit)
{
	ಯದಿ (!can_append(bit))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += 1;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkWire::try_append(DriveChunkWire ಧ್ರುವ &chunk)
{
	ಯದಿ (chunk.wire != wire || chunk.offset != offset + width)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += chunk.width;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkPort::can_append(DriveBitPort ಧ್ರುವ &bit) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

ತಾರ್ಕಿಕ DriveChunkPort::try_append(DriveBitPort ಧ್ರುವ &bit)
{
	ಯದಿ (!can_append(bit))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += 1;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkPort::try_append(DriveChunkPort ಧ್ರುವ &chunk)
{
	ಯದಿ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += chunk.width;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkMarker::can_append(DriveBitMarker ಧ್ರುವ &bit) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ bit.marker == marker && bit.offset == offset + width;
}

ತಾರ್ಕಿಕ DriveChunkMarker::try_append(DriveBitMarker ಧ್ರುವ &bit)
{
	ಯದಿ (!can_append(bit))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += 1;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkMarker::try_append(DriveChunkMarker ಧ್ರುವ &chunk)
{
	ಯದಿ (chunk.marker != marker || chunk.offset != offset + width)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width += chunk.width;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}


ತಾರ್ಕಿಕ DriveChunkMultiple::can_append(DriveBitMultiple ಧ್ರುವ &bit) ಧ್ರುವ
{
	ಯದಿ (bit.multiple().size() != multiple_.size())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪೂರ್ಣಾಙ್ಕ const_drivers = 0;
	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : multiple_)
		ಯದಿ (single.is_constant())
			const_drivers += 1;

	ಯದಿ (const_drivers > 1)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತಿ (DriveBit ಧ್ರುವ &single : bit.multiple())
		ಯದಿ (single.is_constant())
			const_drivers -= 1;

	ಯದಿ (const_drivers != 0)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : multiple_)
	{
		ವಿಕಲ್ಪ (single.type())
		{
			ಪಕ್ಷ DriveType::CONSTANT: {
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE: {
				auto ಧ್ರುವ &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ಯದಿ (!bit.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT: {
				auto ಧ್ರುವ &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ಯದಿ (!bit.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER: {
				auto ಧ್ರುವ &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ಯದಿ (!bit.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkMultiple::can_append(DriveChunkMultiple ಧ್ರುವ &chunk) ಧ್ರುವ
{
	ಯದಿ (chunk.multiple().size() != multiple_.size())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪೂರ್ಣಾಙ್ಕ const_drivers = 0;
	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : multiple_)
		ಯದಿ (single.is_constant())
			const_drivers += 1;

	ಯದಿ (const_drivers > 1)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : chunk.multiple())
		ಯದಿ (single.is_constant())
			const_drivers -= 1;

	ಯದಿ (const_drivers != 0)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : multiple_)
	{
		ವಿಕಲ್ಪ (single.type())
		{
			ಪಕ್ಷ DriveType::CONSTANT: {
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE: {
				auto ಧ್ರುವ &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ಯದಿ (!chunk.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT: {
				auto ಧ್ರುವ &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ಯದಿ (!chunk.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER: {
				auto ಧ್ರುವ &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ಯದಿ (!chunk.multiple().ಎಣಿಕೆ(next))
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			} ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkMultiple::try_append(DriveBitMultiple ಧ್ರುವ &bit)
{
	ಯದಿ (!can_append(bit))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	width_ += 1;
	State constant;

	ಪ್ರತಿ (DriveBit ಧ್ರುವ &single : bit.multiple())
		ಯದಿ (single.is_constant())
			constant = single.constant();

	ಪ್ರತಿ (DriveChunk &single : multiple_)
	{
		ವಿಕಲ್ಪ (single.type())
		{
			ಪಕ್ಷ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE: {
				single.wire().width += 1;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT: {
				single.port().width += 1;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER: {
				single.marker().width += 1;
			} ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				log_abort();
		}
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunkMultiple::try_append(DriveChunkMultiple ಧ್ರುವ &chunk)
{
	ಯದಿ (!can_append(chunk))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪೂರ್ಣಾಙ್ಕ width = chunk.size();
	width_ += width;
	Const constant;

	ಪ್ರತಿ (DriveChunk ಧ್ರುವ &single : chunk.multiple())
		ಯದಿ (single.is_constant())
			constant = single.constant();

	ಪ್ರತಿ (DriveChunk &single : multiple_)
	{
		ವಿಕಲ್ಪ (single.type())
		{
			ಪಕ್ಷ DriveType::CONSTANT: {
				single.constant().append(constant);
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::WIRE: {
				single.wire().width += width;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::PORT: {
				single.port().width += width;
			} ಭಙ್ಗ;
			ಪಕ್ಷ DriveType::MARKER: {
				single.marker().width += width;
			} ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				log_abort();
		}
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ DriveChunk::can_append(DriveBit ಧ್ರುವ &bit) ಧ್ರುವ
{
	ಯದಿ (size() == 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (bit.type() != type_)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ವಿಕಲ್ಪ (type_)
	{
		ಪಕ್ಷ DriveType::NONE:
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::CONSTANT:
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::WIRE:
			ಪ್ರತ್ಯಾಗಮ wire_.can_append(bit.wire());
		ಪಕ್ಷ DriveType::PORT:
			ಪ್ರತ್ಯಾಗಮ port_.can_append(bit.port());
		ಪಕ್ಷ DriveType::MULTIPLE:
			ಪ್ರತ್ಯಾಗಮ multiple_.can_append(bit.multiple());
		ಸ್ವಭಾವ:
			log_abort();
	}
}

ತಾರ್ಕಿಕ DriveChunk::try_append(DriveBit ಧ್ರುವ &bit)
{
	ಯದಿ (size() == 0)
		*ಏತತ್ = bit;
	ಯದಿ (bit.type() != type_)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ವಿಕಲ್ಪ (type_)
	{
		ಪಕ್ಷ DriveType::NONE:
			none_ += 1;
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::WIRE:
			ಪ್ರತ್ಯಾಗಮ wire_.try_append(bit.wire());
		ಪಕ್ಷ DriveType::PORT:
			ಪ್ರತ್ಯಾಗಮ port_.try_append(bit.port());
		ಪಕ್ಷ DriveType::MULTIPLE:
			ಪ್ರತ್ಯಾಗಮ multiple_.try_append(bit.multiple());
		ಸ್ವಭಾವ:
			log_abort();
	}
}


ತಾರ್ಕಿಕ DriveChunk::try_append(DriveChunk ಧ್ರುವ &chunk)
{
	ಯದಿ (size() == 0)
		*ಏತತ್ = chunk;
	ಯದಿ (chunk.type_ != type_)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ವಿಕಲ್ಪ (type_)
	{
		ಪಕ್ಷ DriveType::NONE:
			none_ += chunk.none_;
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪಕ್ಷ DriveType::WIRE:
			ಪ್ರತ್ಯಾಗಮ wire_.try_append(chunk.wire());
		ಪಕ್ಷ DriveType::PORT:
			ಪ್ರತ್ಯಾಗಮ port_.try_append(chunk.port());
		ಪಕ್ಷ DriveType::MARKER:
			ಪ್ರತ್ಯಾಗಮ marker_.try_append(chunk.marker());
		ಪಕ್ಷ DriveType::MULTIPLE:
			ಪ್ರತ್ಯಾಗಮ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

ಶೂನ್ಯ DriveSpec::append(DriveBit ಧ್ರುವ &bit)
{
	hash_ = 0;
	ಯದಿ (!packed()) {
		bits_.ತಳ್ಳು_back(bit);
		width_ += 1;
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

ಶೂನ್ಯ DriveSpec::append(DriveChunk ಧ್ರುವ &chunk)
{
	hash_ = 0;
	pack();
	ಯದಿ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

ಶೂನ್ಯ DriveSpec::pack() ಧ್ರುವ {
	ಯದಿ (bits_.empty())
		ಪ್ರತ್ಯಾಗಮ;
	std::vector<DriveBit> bits(std::move(bits_));
	ಪ್ರತಿ (auto &bit : bits)
		ಯದಿ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

ಶೂನ್ಯ DriveSpec::unpack() ಧ್ರುವ {
	ಯದಿ (chunks_.empty())
		ಪ್ರತ್ಯಾಗಮ;
	ಪ್ರತಿ (auto &chunk : chunks_)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

ಶೂನ್ಯ DriveSpec::compute_width()
{
	width_ = 0;
	ಪ್ರತಿ (auto ಧ್ರುವ &chunk : chunks_)
		width_ += chunk.size();
}

ಶೂನ್ಯ DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ಯದಿ (first_edges.emplace(src, dst).first->second == dst)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (second_edges.emplace(src, dst).first->second == dst)
		ಪ್ರತ್ಯಾಗಮ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::ತೆಗೆ_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ಯದಿ (found_more != more_edges.end()) {
		auto result = found_more->second.ತೆಗೆ();
		ಯದಿ (found_more->second.empty())
			more_edges.erase(found_more);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	auto found_second = second_edges.find(src);
	ಯದಿ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	auto found_first = first_edges.find(src);
	ಯದಿ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಪ್ರತ್ಯಾಗಮ DriveBitId();
}

ಶೂನ್ಯ DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

ತಾರ್ಕಿಕ DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ಪ್ರತ್ಯಾಗಮ first_edges.ಎಣಿಕೆ(src);
}

ಪೂರ್ಣಾಙ್ಕ DriverMap::DriveBitGraph::ಎಣಿಕೆ(DriveBitId src)
{
	ಯದಿ (!first_edges.ಎಣಿಕೆ(src))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (!second_edges.ಎಣಿಕೆ(src))
		ಪ್ರತ್ಯಾಗಮ 1;
	auto found = more_edges.find(src);
	ಯದಿ (found == more_edges.end())
		ಪ್ರತ್ಯಾಗಮ 2;
	ಪ್ರತ್ಯಾಗಮ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ಪೂರ್ಣಾಙ್ಕ index)
{
	ಯದಿ (index == 0)
		ಪ್ರತ್ಯಾಗಮ first_edges.at(src);
	ಅನ್ಯಥಾ ಯದಿ (index == 1)
		ಪ್ರತ್ಯಾಗಮ second_edges.at(src);
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ಧ್ರುವ &bit)
{
	ವಿಕಲ್ಪ (bit.type())
	{
		ಪಕ್ಷ DriveType::NONE:
			ಪ್ರತ್ಯಾಗಮ BitMode::NONE;
		ಪಕ್ಷ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ಪ್ರತ್ಯಾಗಮ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		ಪಕ್ಷ DriveType::WIRE: {
			auto ಧ್ರುವ &wire = bit.wire();
			ತಾರ್ಕಿಕ driver = wire.wire->port_ಒಳಹರಿವು;
			ತಾರ್ಕಿಕ driven = wire.wire->port_output;

			ಯದಿ (driver && !driven)
				ಪ್ರತ್ಯಾಗಮ BitMode::DRIVER;
			ಅನ್ಯಥಾ ಯದಿ (driven && !driver)
				ಪ್ರತ್ಯಾಗಮ BitMode::DRIVEN;
			ಅನ್ಯಥಾ ಯದಿ (driver && driven)
				ಪ್ರತ್ಯಾಗಮ BitMode::TRISTATE;
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		ಪಕ್ಷ DriveType::PORT: {
			auto ಧ್ರುವ &port = bit.port();
			ತಾರ್ಕಿಕ driver = celltypes.cell_output(port.cell->type, port.port);
			ತಾರ್ಕಿಕ driven = celltypes.cell_ಒಳಹರಿವು(port.cell->type, port.port);
			ಯದಿ (driver && !driven)
				ಪ್ರತ್ಯಾಗಮ BitMode::DRIVER;
			ಅನ್ಯಥಾ ಯದಿ (driven && !driver)
				ಪ್ರತ್ಯಾಗಮ BitMode::DRIVEN_UNIQUE;
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ BitMode::TRISTATE;
		}
		ಪಕ್ಷ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		ಸ್ವಭಾವ:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ಧ್ರುವ &bit)
{
	ವಿಕಲ್ಪ (bit.type())
	{
		ಪಕ್ಷ DriveType::NONE:
			ಪ್ರತ್ಯಾಗಮ -1;
		ಪಕ್ಷ DriveType::CONSTANT:
			ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)bit.constant();
		ಪಕ್ಷ DriveType::WIRE: {
			auto ಧ್ರುವ &wire_bit = bit.wire();
			ಪೂರ್ಣಾಙ್ಕ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ಯದಿ (insertion.second) {
				ಯದಿ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ಅನ್ಯಥಾ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ಪ್ರತ್ಯಾಗಮ insertion.first->second.id + wire_bit.offset;
		}
		ಪಕ್ಷ DriveType::PORT: {
			auto ಧ್ರುವ &port_bit = bit.port();
			auto ಕೀಲಿ = std::make_pair(port_bit.cell, port_bit.port);
			ಪೂರ್ಣಾಙ್ಕ offset = next_offset;
			auto insertion = port_offsets.emplace(ಕೀಲಿ, offset);
			ಯದಿ (insertion.second) {
				ಪೂರ್ಣಾಙ್ಕ width = port_bit.cell->connections().at(port_bit.port).size();
				ಯದಿ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ಅನ್ಯಥಾ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ಪ್ರತ್ಯಾಗಮ insertion.first->second.id + port_bit.offset;
		}
		ಸ್ವಭಾವ:
			log_assert(ಅಸತ್ಯ && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ಯದಿ (found_isolated != isolated_drive_bits.end())
		ಪ್ರತ್ಯಾಗಮ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ಯದಿ (found == drive_bits.begin()) {
		ಪ್ರತ್ಯಾಗಮ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ಯದಿ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ಅನ್ಯಥಾ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ಯದಿ (driven_id == driver_id)
		ಪ್ರತ್ಯಾಗಮ;

	same_driver.merge(driven_id, driver_id);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, end = connected_drivers.ಎಣಿಕೆ(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, end = connected_undirected.ಎಣಿಕೆ(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

ಶೂನ್ಯ DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

ಶೂನ್ಯ DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

ಶೂನ್ಯ DriverMap::add(Module *module)
{
	ಪ್ರತಿ (auto ಧ್ರುವ &conn : module->connections())
		add(conn.first, conn.second);

	ಪ್ರತಿ (auto cell : module->cells())
		ಪ್ರತಿ (auto ಧ್ರುವ &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
ಶೂನ್ಯ DriverMap::add(DriveBit ಧ್ರುವ &a, DriveBit ಧ್ರುವ &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	ಯದಿ (a_id == b_id)
		ಪ್ರತ್ಯಾಗಮ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ಯದಿ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ಅನ್ಯಥಾ ಯದಿ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ಅನ್ಯಥಾ ಯದಿ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ಅನ್ಯಥಾ ಯದಿ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ಅನ್ಯಥಾ ಯದಿ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ಅನ್ಯಥಾ ಯದಿ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ಅನ್ಯಥಾ
		connect_undirected(a_id, b_id);

	ಪ್ರತ್ಯಾಗಮ;
}

// Specialized version that avoids unpacking
ಶೂನ್ಯ DriverMap::add(SigSpec ಧ್ರುವ &a, SigSpec ಧ್ರುವ &b)
{
	log_assert(a.size() == b.size());
	auto ಧ್ರುವ &a_chunks = a.chunks();
	auto ಧ್ರುವ &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ಪೂರ್ಣಾಙ್ಕ a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ಪೂರ್ಣಾಙ್ಕ b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ಯಾವತ್ (a_chunk != a_end) {
		ಪೂರ್ಣಾಙ್ಕ a_width = a_chunk->width - a_offset;
		ಯದಿ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			ಅನುವರ್ತ;
		}
		ಪೂರ್ಣಾಙ್ಕ b_width = b_chunk->width - b_offset;
		ಯದಿ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			ಅನುವರ್ತ;
		}
		ಪೂರ್ಣಾಙ್ಕ width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ಧ್ರುವ &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ಧ್ರುವ &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

ಶೂನ್ಯ DriverMap::add_port(Cell *cell, IdString port, SigSpec ಧ್ರುವ &b)
{
	ಪೂರ್ಣಾಙ್ಕ offset = 0;
	ಪ್ರತಿ (auto ಧ್ರುವ &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

ಶೂನ್ಯ DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ಪೂರ್ಣಾಙ್ಕ> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ಯದಿ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ಯದಿ (connected_drivers.contains(current))
			drivers.emplace(current);

		ಪೂರ್ಣಾಙ್ಕ undirected_driver_ಎಣಿಕೆ = connected_undirected.ಎಣಿಕೆ(current);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != undirected_driver_ಎಣಿಕೆ; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ಯದಿ (drivers.empty())
		ಪ್ರತಿ (auto seen_id : seen)
			drivers.emplace(seen_id);

	ಪ್ರತಿ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ಪೂರ್ಣಾಙ್ಕ undirected_driver_ಎಣಿಕೆ = connected_undirected.ಎಣಿಕೆ(current);

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != undirected_driver_ಎಣಿಕೆ; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ಯದಿ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	ಪ್ರತಿ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ಧ್ರುವ &bit)
{
	ಯದಿ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ಪ್ರತ್ಯಾಗಮ bit;
	ಯದಿ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		ಪ್ರತಿ (auto ಧ್ರುವ &inner : bit.multiple().multiple())
			result.merge((*ಏತತ್)(inner));
		ಪ್ರತ್ಯಾಗಮ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ಯದಿ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ಪ್ರತ್ಯಾಗಮ bit_repr;

	ಪೂರ್ಣಾಙ್ಕ implicit_driver_ಎಣಿಕೆ = connected_drivers.ಎಣಿಕೆ(bit_repr_id);
	ಯದಿ (connected_undirected.contains(bit_repr_id) && !oriented_present.ಎಣಿಕೆ(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ಯದಿ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != implicit_driver_ಎಣಿಕೆ; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ಪೂರ್ಣಾಙ್ಕ oriented_driver_ಎಣಿಕೆ = connected_oriented.ಎಣಿಕೆ(bit_repr_id);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != oriented_driver_ಎಣಿಕೆ; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ಪ್ರತ್ಯಾಗಮ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, width = spec.size(); i != width; ++i)
		result.append((*ಏತತ್)(spec[i]));

	ಪ್ರತ್ಯಾಗಮ result;
}

std::string log_signal(DriveChunkWire ಧ್ರುವ &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ಯದಿ (chunk.is_whole())
		ಪ್ರತ್ಯಾಗಮ id;
	ಯದಿ (chunk.width == 1)
		ಪ್ರತ್ಯಾಗಮ stringf("%s [%d]", id, chunk.offset);
	ಪ್ರತ್ಯಾಗಮ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ಧ್ರುವ &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ಯದಿ (chunk.is_whole())
		ಪ್ರತ್ಯಾಗಮ stringf("%s <%s>", cell_id, port_id);
	ಯದಿ (chunk.width == 1)
		ಪ್ರತ್ಯಾಗಮ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ಪ್ರತ್ಯಾಗಮ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ಧ್ರುವ &chunk)
{
	ಯದಿ (chunk.width == 1)
		ಪ್ರತ್ಯಾಗಮ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ಪ್ರತ್ಯಾಗಮ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ಧ್ರುವ &chunk)
{
	ವಿಕಲ್ಪ (chunk.type())
	{
		ಪಕ್ಷ DriveType::NONE:
			ಪ್ರತ್ಯಾಗಮ stringf("<none x%d>", chunk.size());
		ಪಕ್ಷ DriveType::CONSTANT:
			ಪ್ರತ್ಯಾಗಮ log_const(chunk.constant());
		ಪಕ್ಷ DriveType::WIRE:
			ಪ್ರತ್ಯಾಗಮ log_signal(chunk.wire());
		ಪಕ್ಷ DriveType::PORT:
			ಪ್ರತ್ಯಾಗಮ log_signal(chunk.port());
		ಪಕ್ಷ DriveType::MARKER:
			ಪ್ರತ್ಯಾಗಮ log_signal(chunk.marker());
		ಪಕ್ಷ DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ಧ್ರುವ ವರ್ಣ *sep = " ";
			ಪ್ರತಿ (auto ಧ್ರುವ &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ಪ್ರತ್ಯಾಗಮ str;
		}
		ಸ್ವಭಾವ:
			log_abort();
	}
}

std::string log_signal(DriveSpec ಧ್ರುವ &spec)
{
	auto &chunks = spec.chunks();
	ಯದಿ (chunks.empty())
		ಪ್ರತ್ಯಾಗಮ "{}";
	ಯದಿ (chunks.size() == 1)
		ಪ್ರತ್ಯಾಗಮ log_signal(chunks[0]);

	std::string str;
	ಧ್ರುವ ವರ್ಣ *sep = "{ ";

	ಪ್ರತಿ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ಪ್ರತ್ಯಾಗಮ str;
}

YOSYS_NAMESPACE_END
