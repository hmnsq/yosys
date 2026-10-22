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

#ଅନ୍ତର୍ଭାଵ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit ଅଚଳ &bit)
{
	ଯଦି (bit.is_wire())
		*ଏହି = DriveBitWire(bit.wire, bit.offset);
	ଅନ୍ୟଥା
		*ଏହି = bit.data;
}

ଶୂନ୍ୟ DriveBit::merge(DriveBit ଅଚଳ &other)
{
	ଯଦି (other.type_ == DriveType::NONE)
		ଫେରନ୍ତୁ;
	ଯଦି (type_ == DriveType::NONE) {
		*ଏହି = other;
		ଫେରନ୍ତୁ;
	}
	ଯଦି (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*ଏହି));
		*ଏହି = std::move(multi);
	}
	multiple().merge(other);
}


ଶୂନ୍ୟ DriveBitMultiple::merge(DriveBit ଅଚଳ &single)
{
	ଯଦି (single.type() == DriveType::NONE)
		ଫେରନ୍ତୁ;
	ଯଦି (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ଫେରନ୍ତୁ;
	}
	multiple_.emplace(single);
}

ଶୂନ୍ୟ DriveBitMultiple::merge(DriveBit &&single)
{
	ଯଦି (single.type() == DriveType::NONE)
		ଫେରନ୍ତୁ;
	ଯଦି (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ଫେରନ୍ତୁ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) ଅଚଳ
{
	DriveBitMultiple result;
	ପାଇଁ (auto ଅଚଳ &single : multiple_)
		result.merge(single[i]);
	ଫେରନ୍ତୁ result;
}


ତାର୍କିକ DriveChunkWire::can_append(DriveBitWire ଅଚଳ &bit) ଅଚଳ
{
	ଫେରନ୍ତୁ bit.wire == wire && bit.offset == offset + width;
}

ତାର୍କିକ DriveChunkWire::try_append(DriveBitWire ଅଚଳ &bit)
{
	ଯଦି (!can_append(bit))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += 1;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkWire::try_append(DriveChunkWire ଅଚଳ &chunk)
{
	ଯଦି (chunk.wire != wire || chunk.offset != offset + width)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += chunk.width;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkPort::can_append(DriveBitPort ଅଚଳ &bit) ଅଚଳ
{
	ଫେରନ୍ତୁ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

ତାର୍କିକ DriveChunkPort::try_append(DriveBitPort ଅଚଳ &bit)
{
	ଯଦି (!can_append(bit))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += 1;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkPort::try_append(DriveChunkPort ଅଚଳ &chunk)
{
	ଯଦି (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += chunk.width;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkMarker::can_append(DriveBitMarker ଅଚଳ &bit) ଅଚଳ
{
	ଫେରନ୍ତୁ bit.marker == marker && bit.offset == offset + width;
}

ତାର୍କିକ DriveChunkMarker::try_append(DriveBitMarker ଅଚଳ &bit)
{
	ଯଦି (!can_append(bit))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += 1;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkMarker::try_append(DriveChunkMarker ଅଚଳ &chunk)
{
	ଯଦି (chunk.marker != marker || chunk.offset != offset + width)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width += chunk.width;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}


ତାର୍କିକ DriveChunkMultiple::can_append(DriveBitMultiple ଅଚଳ &bit) ଅଚଳ
{
	ଯଦି (bit.multiple().size() != multiple_.size())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା const_drivers = 0;
	ପାଇଁ (DriveChunk ଅଚଳ &single : multiple_)
		ଯଦି (single.is_constant())
			const_drivers += 1;

	ଯଦି (const_drivers > 1)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପାଇଁ (DriveBit ଅଚଳ &single : bit.multiple())
		ଯଦି (single.is_constant())
			const_drivers -= 1;

	ଯଦି (const_drivers != 0)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପାଇଁ (DriveChunk ଅଚଳ &single : multiple_)
	{
		ବାଛ (single.type())
		{
			ସ୍ଥିତି DriveType::CONSTANT: {
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE: {
				auto ଅଚଳ &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ଯଦି (!bit.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT: {
				auto ଅଚଳ &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ଯଦି (!bit.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER: {
				auto ଅଚଳ &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ଯଦି (!bit.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkMultiple::can_append(DriveChunkMultiple ଅଚଳ &chunk) ଅଚଳ
{
	ଯଦି (chunk.multiple().size() != multiple_.size())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା const_drivers = 0;
	ପାଇଁ (DriveChunk ଅଚଳ &single : multiple_)
		ଯଦି (single.is_constant())
			const_drivers += 1;

	ଯଦି (const_drivers > 1)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପାଇଁ (DriveChunk ଅଚଳ &single : chunk.multiple())
		ଯଦି (single.is_constant())
			const_drivers -= 1;

	ଯଦି (const_drivers != 0)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପାଇଁ (DriveChunk ଅଚଳ &single : multiple_)
	{
		ବାଛ (single.type())
		{
			ସ୍ଥିତି DriveType::CONSTANT: {
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE: {
				auto ଅଚଳ &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ଯଦି (!chunk.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT: {
				auto ଅଚଳ &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ଯଦି (!chunk.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER: {
				auto ଅଚଳ &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ଯଦି (!chunk.multiple().ଗଣନା_2(next))
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
			} ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkMultiple::try_append(DriveBitMultiple ଅଚଳ &bit)
{
	ଯଦି (!can_append(bit))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	width_ += 1;
	State constant;

	ପାଇଁ (DriveBit ଅଚଳ &single : bit.multiple())
		ଯଦି (single.is_constant())
			constant = single.constant();

	ପାଇଁ (DriveChunk &single : multiple_)
	{
		ବାଛ (single.type())
		{
			ସ୍ଥିତି DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE: {
				single.wire().width += 1;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT: {
				single.port().width += 1;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER: {
				single.marker().width += 1;
			} ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				log_abort();
		}
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunkMultiple::try_append(DriveChunkMultiple ଅଚଳ &chunk)
{
	ଯଦି (!can_append(chunk))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = chunk.size();
	width_ += width;
	Const constant;

	ପାଇଁ (DriveChunk ଅଚଳ &single : chunk.multiple())
		ଯଦି (single.is_constant())
			constant = single.constant();

	ପାଇଁ (DriveChunk &single : multiple_)
	{
		ବାଛ (single.type())
		{
			ସ୍ଥିତି DriveType::CONSTANT: {
				single.constant().append(constant);
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::WIRE: {
				single.wire().width += width;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::PORT: {
				single.port().width += width;
			} ଭାଙ୍ଗ;
			ସ୍ଥିତି DriveType::MARKER: {
				single.marker().width += width;
			} ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				log_abort();
		}
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ DriveChunk::can_append(DriveBit ଅଚଳ &bit) ଅଚଳ
{
	ଯଦି (size() == 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (bit.type() != type_)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ବାଛ (type_)
	{
		ସ୍ଥିତି DriveType::NONE:
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::CONSTANT:
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::WIRE:
			ଫେରନ୍ତୁ wire_.can_append(bit.wire());
		ସ୍ଥିତି DriveType::PORT:
			ଫେରନ୍ତୁ port_.can_append(bit.port());
		ସ୍ଥିତି DriveType::MULTIPLE:
			ଫେରନ୍ତୁ multiple_.can_append(bit.multiple());
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			log_abort();
	}
}

ତାର୍କିକ DriveChunk::try_append(DriveBit ଅଚଳ &bit)
{
	ଯଦି (size() == 0)
		*ଏହି = bit;
	ଯଦି (bit.type() != type_)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ବାଛ (type_)
	{
		ସ୍ଥିତି DriveType::NONE:
			none_ += 1;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::WIRE:
			ଫେରନ୍ତୁ wire_.try_append(bit.wire());
		ସ୍ଥିତି DriveType::PORT:
			ଫେରନ୍ତୁ port_.try_append(bit.port());
		ସ୍ଥିତି DriveType::MULTIPLE:
			ଫେରନ୍ତୁ multiple_.try_append(bit.multiple());
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			log_abort();
	}
}


ତାର୍କିକ DriveChunk::try_append(DriveChunk ଅଚଳ &chunk)
{
	ଯଦି (size() == 0)
		*ଏହି = chunk;
	ଯଦି (chunk.type_ != type_)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ବାଛ (type_)
	{
		ସ୍ଥିତି DriveType::NONE:
			none_ += chunk.none_;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ସ୍ଥିତି DriveType::WIRE:
			ଫେରନ୍ତୁ wire_.try_append(chunk.wire());
		ସ୍ଥିତି DriveType::PORT:
			ଫେରନ୍ତୁ port_.try_append(chunk.port());
		ସ୍ଥିତି DriveType::MARKER:
			ଫେରନ୍ତୁ marker_.try_append(chunk.marker());
		ସ୍ଥିତି DriveType::MULTIPLE:
			ଫେରନ୍ତୁ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

ଶୂନ୍ୟ DriveSpec::append(DriveBit ଅଚଳ &bit)
{
	hash_ = 0;
	ଯଦି (!packed()) {
		bits_.ଠେଲ_back(bit);
		width_ += 1;
		ଫେରନ୍ତୁ;
	}

	ଯଦି (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

ଶୂନ୍ୟ DriveSpec::append(DriveChunk ଅଚଳ &chunk)
{
	hash_ = 0;
	pack();
	ଯଦି (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

ଶୂନ୍ୟ DriveSpec::pack() ଅଚଳ {
	ଯଦି (bits_.empty())
		ଫେରନ୍ତୁ;
	std::vector<DriveBit> bits(std::move(bits_));
	ପାଇଁ (auto &bit : bits)
		ଯଦି (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

ଶୂନ୍ୟ DriveSpec::unpack() ଅଚଳ {
	ଯଦି (chunks_.empty())
		ଫେରନ୍ତୁ;
	ପାଇଁ (auto &chunk : chunks_)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

ଶୂନ୍ୟ DriveSpec::compute_width()
{
	width_ = 0;
	ପାଇଁ (auto ଅଚଳ &chunk : chunks_)
		width_ += chunk.size();
}

ଶୂନ୍ୟ DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ଯଦି (first_edges.emplace(src, dst).first->second == dst)
		ଫେରନ୍ତୁ;
	ଯଦି (second_edges.emplace(src, dst).first->second == dst)
		ଫେରନ୍ତୁ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::କାଢ଼_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ଯଦି (found_more != more_edges.end()) {
		auto result = found_more->second.କାଢ଼();
		ଯଦି (found_more->second.empty())
			more_edges.erase(found_more);
		ଫେରନ୍ତୁ result;
	}

	auto found_second = second_edges.find(src);
	ଯଦି (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ଫେରନ୍ତୁ result;
	}

	auto found_first = first_edges.find(src);
	ଯଦି (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ଫେରନ୍ତୁ result;
	}

	ଫେରନ୍ତୁ DriveBitId();
}

ଶୂନ୍ୟ DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

ତାର୍କିକ DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ଫେରନ୍ତୁ first_edges.ଗଣନା_2(src);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା DriverMap::DriveBitGraph::ଗଣନା_2(DriveBitId src)
{
	ଯଦି (!first_edges.ଗଣନା_2(src))
		ଫେରନ୍ତୁ 0;
	ଯଦି (!second_edges.ଗଣନା_2(src))
		ଫେରନ୍ତୁ 1;
	auto found = more_edges.find(src);
	ଯଦି (found == more_edges.end())
		ଫେରନ୍ତୁ 2;
	ଫେରନ୍ତୁ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
{
	ଯଦି (index == 0)
		ଫେରନ୍ତୁ first_edges.at(src);
	ଅନ୍ୟଥା ଯଦି (index == 1)
		ଫେରନ୍ତୁ second_edges.at(src);
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ଅଚଳ &bit)
{
	ବାଛ (bit.type())
	{
		ସ୍ଥିତି DriveType::NONE:
			ଫେରନ୍ତୁ BitMode::NONE;
		ସ୍ଥିତି DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ଫେରନ୍ତୁ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		ସ୍ଥିତି DriveType::WIRE: {
			auto ଅଚଳ &wire = bit.wire();
			ତାର୍କିକ driver = wire.wire->port_ନିବେଶ;
			ତାର୍କିକ driven = wire.wire->port_output;

			ଯଦି (driver && !driven)
				ଫେରନ୍ତୁ BitMode::DRIVER;
			ଅନ୍ୟଥା ଯଦି (driven && !driver)
				ଫେରନ୍ତୁ BitMode::DRIVEN;
			ଅନ୍ୟଥା ଯଦି (driver && driven)
				ଫେରନ୍ତୁ BitMode::TRISTATE;
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		ସ୍ଥିତି DriveType::PORT: {
			auto ଅଚଳ &port = bit.port();
			ତାର୍କିକ driver = celltypes.cell_output(port.cell->type, port.port);
			ତାର୍କିକ driven = celltypes.cell_ନିବେଶ(port.cell->type, port.port);
			ଯଦି (driver && !driven)
				ଫେରନ୍ତୁ BitMode::DRIVER;
			ଅନ୍ୟଥା ଯଦି (driven && !driver)
				ଫେରନ୍ତୁ BitMode::DRIVEN_UNIQUE;
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ BitMode::TRISTATE;
		}
		ସ୍ଥିତି DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ଅଚଳ &bit)
{
	ବାଛ (bit.type())
	{
		ସ୍ଥିତି DriveType::NONE:
			ଫେରନ୍ତୁ -1;
		ସ୍ଥିତି DriveType::CONSTANT:
			ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bit.constant();
		ସ୍ଥିତି DriveType::WIRE: {
			auto ଅଚଳ &wire_bit = bit.wire();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ଯଦି (insertion.second) {
				ଯଦି (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ଅନ୍ୟଥା
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ଫେରନ୍ତୁ insertion.first->second.id + wire_bit.offset;
		}
		ସ୍ଥିତି DriveType::PORT: {
			auto ଅଚଳ &port_bit = bit.port();
			auto ଚାବି = std::make_pair(port_bit.cell, port_bit.port);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = next_offset;
			auto insertion = port_offsets.emplace(ଚାବି, offset);
			ଯଦି (insertion.second) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = port_bit.cell->connections().at(port_bit.port).size();
				ଯଦି (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ଅନ୍ୟଥା
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ଫେରନ୍ତୁ insertion.first->second.id + port_bit.offset;
		}
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			log_assert(ମିଥ୍ୟା && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ଯଦି (found_isolated != isolated_drive_bits.end())
		ଫେରନ୍ତୁ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ଯଦି (found == drive_bits.begin()) {
		ଫେରନ୍ତୁ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ଯଦି (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ଅନ୍ୟଥା {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ଯଦି (driven_id == driver_id)
		ଫେରନ୍ତୁ;

	same_driver.merge(driven_id, driver_id);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, end = connected_drivers.ଗଣନା_2(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, end = connected_undirected.ଗଣନା_2(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

ଶୂନ୍ୟ DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

ଶୂନ୍ୟ DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

ଶୂନ୍ୟ DriverMap::add(Module *module)
{
	ପାଇଁ (auto ଅଚଳ &conn : module->connections())
		add(conn.first, conn.second);

	ପାଇଁ (auto cell : module->cells())
		ପାଇଁ (auto ଅଚଳ &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
ଶୂନ୍ୟ DriverMap::add(DriveBit ଅଚଳ &a, DriveBit ଅଚଳ &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	ଯଦି (a_id == b_id)
		ଫେରନ୍ତୁ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ଯଦି (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ଅନ୍ୟଥା ଯଦି (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ଅନ୍ୟଥା ଯଦି (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ଅନ୍ୟଥା ଯଦି (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ଅନ୍ୟଥା ଯଦି (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ଅନ୍ୟଥା ଯଦି (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ଅନ୍ୟଥା
		connect_undirected(a_id, b_id);

	ଫେରନ୍ତୁ;
}

// Specialized version that avoids unpacking
ଶୂନ୍ୟ DriverMap::add(SigSpec ଅଚଳ &a, SigSpec ଅଚଳ &b)
{
	log_assert(a.size() == b.size());
	auto ଅଚଳ &a_chunks = a.chunks();
	auto ଅଚଳ &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ଯେତେବେଳେ (a_chunk != a_end) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = a_chunk->width - a_offset;
		ଯଦି (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			ଜାରି;
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = b_chunk->width - b_offset;
		ଯଦି (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			ଜାରି;
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ଅଚଳ &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ଅଚଳ &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

ଶୂନ୍ୟ DriverMap::add_port(Cell *cell, IdString port, SigSpec ଅଚଳ &b)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = 0;
	ପାଇଁ (auto ଅଚଳ &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

ଶୂନ୍ୟ DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ଯଦି (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ଯଦି (connected_drivers.contains(current))
			drivers.emplace(current);

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା undirected_driver_ଗଣନା = connected_undirected.ଗଣନା_2(current);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != undirected_driver_ଗଣନା; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ଯଦି (drivers.empty())
		ପାଇଁ (auto seen_id : seen)
			drivers.emplace(seen_id);

	ପାଇଁ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା undirected_driver_ଗଣନା = connected_undirected.ଗଣନା_2(current);

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != undirected_driver_ଗଣନା; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ଯଦି (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	ପାଇଁ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ଅଚଳ &bit)
{
	ଯଦି (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ଫେରନ୍ତୁ bit;
	ଯଦି (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		ପାଇଁ (auto ଅଚଳ &inner : bit.multiple().multiple())
			result.merge((*ଏହି)(inner));
		ଫେରନ୍ତୁ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ଯଦି (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ଫେରନ୍ତୁ bit_repr;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା implicit_driver_ଗଣନା = connected_drivers.ଗଣନା_2(bit_repr_id);
	ଯଦି (connected_undirected.contains(bit_repr_id) && !oriented_present.ଗଣନା_2(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ଯଦି (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != implicit_driver_ଗଣନା; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା oriented_driver_ଗଣନା = connected_oriented.ଗଣନା_2(bit_repr_id);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != oriented_driver_ଗଣନା; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ଫେରନ୍ତୁ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, width = spec.size(); i != width; ++i)
		result.append((*ଏହି)(spec[i]));

	ଫେରନ୍ତୁ result;
}

std::string log_signal(DriveChunkWire ଅଚଳ &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ଯଦି (chunk.is_whole())
		ଫେରନ୍ତୁ id;
	ଯଦି (chunk.width == 1)
		ଫେରନ୍ତୁ stringf("%s [%d]", id, chunk.offset);
	ଫେରନ୍ତୁ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ଅଚଳ &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ଯଦି (chunk.is_whole())
		ଫେରନ୍ତୁ stringf("%s <%s>", cell_id, port_id);
	ଯଦି (chunk.width == 1)
		ଫେରନ୍ତୁ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ଫେରନ୍ତୁ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ଅଚଳ &chunk)
{
	ଯଦି (chunk.width == 1)
		ଫେରନ୍ତୁ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ଫେରନ୍ତୁ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ଅଚଳ &chunk)
{
	ବାଛ (chunk.type())
	{
		ସ୍ଥିତି DriveType::NONE:
			ଫେରନ୍ତୁ stringf("<none x%d>", chunk.size());
		ସ୍ଥିତି DriveType::CONSTANT:
			ଫେରନ୍ତୁ log_const(chunk.constant());
		ସ୍ଥିତି DriveType::WIRE:
			ଫେରନ୍ତୁ log_signal(chunk.wire());
		ସ୍ଥିତି DriveType::PORT:
			ଫେରନ୍ତୁ log_signal(chunk.port());
		ସ୍ଥିତି DriveType::MARKER:
			ଫେରନ୍ତୁ log_signal(chunk.marker());
		ସ୍ଥିତି DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ଅଚଳ ଅକ୍ଷର *sep = " ";
			ପାଇଁ (auto ଅଚଳ &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ଫେରନ୍ତୁ str;
		}
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			log_abort();
	}
}

std::string log_signal(DriveSpec ଅଚଳ &spec)
{
	auto &chunks = spec.chunks();
	ଯଦି (chunks.empty())
		ଫେରନ୍ତୁ "{}";
	ଯଦି (chunks.size() == 1)
		ଫେରନ୍ତୁ log_signal(chunks[0]);

	std::string str;
	ଅଚଳ ଅକ୍ଷର *sep = "{ ";

	ପାଇଁ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ଫେରନ୍ତୁ str;
}

YOSYS_NAMESPACE_END
