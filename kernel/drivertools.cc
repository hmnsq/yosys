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

#അന്തര്ഭാവ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit സ്ഥിരം &bit)
{
	എങ്കിൽ (bit.is_wire())
		*ഇത് = DriveBitWire(bit.wire, bit.offset);
	അല്ലെങ്കിൽ
		*ഇത് = bit.data;
}

ശൂന്യം DriveBit::merge(DriveBit സ്ഥിരം &other)
{
	എങ്കിൽ (other.type_ == DriveType::NONE)
		തിരികെ;
	എങ്കിൽ (type_ == DriveType::NONE) {
		*ഇത് = other;
		തിരികെ;
	}
	എങ്കിൽ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*ഇത്));
		*ഇത് = std::move(multi);
	}
	multiple().merge(other);
}


ശൂന്യം DriveBitMultiple::merge(DriveBit സ്ഥിരം &single)
{
	എങ്കിൽ (single.type() == DriveType::NONE)
		തിരികെ;
	എങ്കിൽ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		തിരികെ;
	}
	multiple_.emplace(single);
}

ശൂന്യം DriveBitMultiple::merge(DriveBit &&single)
{
	എങ്കിൽ (single.type() == DriveType::NONE)
		തിരികെ;
	എങ്കിൽ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		തിരികെ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](പൂർണ്ണസംഖ്യ i) സ്ഥിരം
{
	DriveBitMultiple result;
	വേണ്ടി (auto സ്ഥിരം &single : multiple_)
		result.merge(single[i]);
	തിരികെ result;
}


തർക്കം DriveChunkWire::can_append(DriveBitWire സ്ഥിരം &bit) സ്ഥിരം
{
	തിരികെ bit.wire == wire && bit.offset == offset + width;
}

തർക്കം DriveChunkWire::try_append(DriveBitWire സ്ഥിരം &bit)
{
	എങ്കിൽ (!can_append(bit))
		തിരികെ അസത്യം;
	width += 1;
	തിരികെ സത്യം;
}

തർക്കം DriveChunkWire::try_append(DriveChunkWire സ്ഥിരം &chunk)
{
	എങ്കിൽ (chunk.wire != wire || chunk.offset != offset + width)
		തിരികെ അസത്യം;
	width += chunk.width;
	തിരികെ സത്യം;
}

തർക്കം DriveChunkPort::can_append(DriveBitPort സ്ഥിരം &bit) സ്ഥിരം
{
	തിരികെ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

തർക്കം DriveChunkPort::try_append(DriveBitPort സ്ഥിരം &bit)
{
	എങ്കിൽ (!can_append(bit))
		തിരികെ അസത്യം;
	width += 1;
	തിരികെ സത്യം;
}

തർക്കം DriveChunkPort::try_append(DriveChunkPort സ്ഥിരം &chunk)
{
	എങ്കിൽ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		തിരികെ അസത്യം;
	width += chunk.width;
	തിരികെ സത്യം;
}

തർക്കം DriveChunkMarker::can_append(DriveBitMarker സ്ഥിരം &bit) സ്ഥിരം
{
	തിരികെ bit.marker == marker && bit.offset == offset + width;
}

തർക്കം DriveChunkMarker::try_append(DriveBitMarker സ്ഥിരം &bit)
{
	എങ്കിൽ (!can_append(bit))
		തിരികെ അസത്യം;
	width += 1;
	തിരികെ സത്യം;
}

തർക്കം DriveChunkMarker::try_append(DriveChunkMarker സ്ഥിരം &chunk)
{
	എങ്കിൽ (chunk.marker != marker || chunk.offset != offset + width)
		തിരികെ അസത്യം;
	width += chunk.width;
	തിരികെ സത്യം;
}


തർക്കം DriveChunkMultiple::can_append(DriveBitMultiple സ്ഥിരം &bit) സ്ഥിരം
{
	എങ്കിൽ (bit.multiple().size() != multiple_.size())
		തിരികെ അസത്യം;

	പൂർണ്ണസംഖ്യ const_drivers = 0;
	വേണ്ടി (DriveChunk സ്ഥിരം &single : multiple_)
		എങ്കിൽ (single.is_constant())
			const_drivers += 1;

	എങ്കിൽ (const_drivers > 1)
		തിരികെ അസത്യം;

	വേണ്ടി (DriveBit സ്ഥിരം &single : bit.multiple())
		എങ്കിൽ (single.is_constant())
			const_drivers -= 1;

	എങ്കിൽ (const_drivers != 0)
		തിരികെ അസത്യം;

	വേണ്ടി (DriveChunk സ്ഥിരം &single : multiple_)
	{
		തിരഞ്ഞെടുപ്പ് (single.type())
		{
			അവസ്ഥ DriveType::CONSTANT: {
			} നിർത്തുക;
			അവസ്ഥ DriveType::WIRE: {
				auto സ്ഥിരം &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				എങ്കിൽ (!bit.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			അവസ്ഥ DriveType::PORT: {
				auto സ്ഥിരം &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				എങ്കിൽ (!bit.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			അവസ്ഥ DriveType::MARKER: {
				auto സ്ഥിരം &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				എങ്കിൽ (!bit.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				തിരികെ അസത്യം;
		}
	}

	തിരികെ സത്യം;
}

തർക്കം DriveChunkMultiple::can_append(DriveChunkMultiple സ്ഥിരം &chunk) സ്ഥിരം
{
	എങ്കിൽ (chunk.multiple().size() != multiple_.size())
		തിരികെ അസത്യം;

	പൂർണ്ണസംഖ്യ const_drivers = 0;
	വേണ്ടി (DriveChunk സ്ഥിരം &single : multiple_)
		എങ്കിൽ (single.is_constant())
			const_drivers += 1;

	എങ്കിൽ (const_drivers > 1)
		തിരികെ അസത്യം;

	വേണ്ടി (DriveChunk സ്ഥിരം &single : chunk.multiple())
		എങ്കിൽ (single.is_constant())
			const_drivers -= 1;

	എങ്കിൽ (const_drivers != 0)
		തിരികെ അസത്യം;

	വേണ്ടി (DriveChunk സ്ഥിരം &single : multiple_)
	{
		തിരഞ്ഞെടുപ്പ് (single.type())
		{
			അവസ്ഥ DriveType::CONSTANT: {
			} നിർത്തുക;
			അവസ്ഥ DriveType::WIRE: {
				auto സ്ഥിരം &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				എങ്കിൽ (!chunk.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			അവസ്ഥ DriveType::PORT: {
				auto സ്ഥിരം &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				എങ്കിൽ (!chunk.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			അവസ്ഥ DriveType::MARKER: {
				auto സ്ഥിരം &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				എങ്കിൽ (!chunk.multiple().എണ്ണം(next))
					തിരികെ അസത്യം;
			} നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				തിരികെ അസത്യം;
		}
	}

	തിരികെ സത്യം;
}

തർക്കം DriveChunkMultiple::try_append(DriveBitMultiple സ്ഥിരം &bit)
{
	എങ്കിൽ (!can_append(bit))
		തിരികെ അസത്യം;
	width_ += 1;
	State constant;

	വേണ്ടി (DriveBit സ്ഥിരം &single : bit.multiple())
		എങ്കിൽ (single.is_constant())
			constant = single.constant();

	വേണ്ടി (DriveChunk &single : multiple_)
	{
		തിരഞ്ഞെടുപ്പ് (single.type())
		{
			അവസ്ഥ DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} നിർത്തുക;
			അവസ്ഥ DriveType::WIRE: {
				single.wire().width += 1;
			} നിർത്തുക;
			അവസ്ഥ DriveType::PORT: {
				single.port().width += 1;
			} നിർത്തുക;
			അവസ്ഥ DriveType::MARKER: {
				single.marker().width += 1;
			} നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				log_abort();
		}
	}
	തിരികെ സത്യം;
}

തർക്കം DriveChunkMultiple::try_append(DriveChunkMultiple സ്ഥിരം &chunk)
{
	എങ്കിൽ (!can_append(chunk))
		തിരികെ അസത്യം;
	പൂർണ്ണസംഖ്യ width = chunk.size();
	width_ += width;
	Const constant;

	വേണ്ടി (DriveChunk സ്ഥിരം &single : chunk.multiple())
		എങ്കിൽ (single.is_constant())
			constant = single.constant();

	വേണ്ടി (DriveChunk &single : multiple_)
	{
		തിരഞ്ഞെടുപ്പ് (single.type())
		{
			അവസ്ഥ DriveType::CONSTANT: {
				single.constant().append(constant);
			} നിർത്തുക;
			അവസ്ഥ DriveType::WIRE: {
				single.wire().width += width;
			} നിർത്തുക;
			അവസ്ഥ DriveType::PORT: {
				single.port().width += width;
			} നിർത്തുക;
			അവസ്ഥ DriveType::MARKER: {
				single.marker().width += width;
			} നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				log_abort();
		}
	}
	തിരികെ സത്യം;
}

തർക്കം DriveChunk::can_append(DriveBit സ്ഥിരം &bit) സ്ഥിരം
{
	എങ്കിൽ (size() == 0)
		തിരികെ സത്യം;
	എങ്കിൽ (bit.type() != type_)
		തിരികെ അസത്യം;
	തിരഞ്ഞെടുപ്പ് (type_)
	{
		അവസ്ഥ DriveType::NONE:
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::CONSTANT:
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::WIRE:
			തിരികെ wire_.can_append(bit.wire());
		അവസ്ഥ DriveType::PORT:
			തിരികെ port_.can_append(bit.port());
		അവസ്ഥ DriveType::MULTIPLE:
			തിരികെ multiple_.can_append(bit.multiple());
		സ്ഥിരസ്ഥിതി:
			log_abort();
	}
}

തർക്കം DriveChunk::try_append(DriveBit സ്ഥിരം &bit)
{
	എങ്കിൽ (size() == 0)
		*ഇത് = bit;
	എങ്കിൽ (bit.type() != type_)
		തിരികെ അസത്യം;
	തിരഞ്ഞെടുപ്പ് (type_)
	{
		അവസ്ഥ DriveType::NONE:
			none_ += 1;
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::WIRE:
			തിരികെ wire_.try_append(bit.wire());
		അവസ്ഥ DriveType::PORT:
			തിരികെ port_.try_append(bit.port());
		അവസ്ഥ DriveType::MULTIPLE:
			തിരികെ multiple_.try_append(bit.multiple());
		സ്ഥിരസ്ഥിതി:
			log_abort();
	}
}


തർക്കം DriveChunk::try_append(DriveChunk സ്ഥിരം &chunk)
{
	എങ്കിൽ (size() == 0)
		*ഇത് = chunk;
	എങ്കിൽ (chunk.type_ != type_)
		തിരികെ അസത്യം;
	തിരഞ്ഞെടുപ്പ് (type_)
	{
		അവസ്ഥ DriveType::NONE:
			none_ += chunk.none_;
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			തിരികെ സത്യം;
		അവസ്ഥ DriveType::WIRE:
			തിരികെ wire_.try_append(chunk.wire());
		അവസ്ഥ DriveType::PORT:
			തിരികെ port_.try_append(chunk.port());
		അവസ്ഥ DriveType::MARKER:
			തിരികെ marker_.try_append(chunk.marker());
		അവസ്ഥ DriveType::MULTIPLE:
			തിരികെ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

ശൂന്യം DriveSpec::append(DriveBit സ്ഥിരം &bit)
{
	hash_ = 0;
	എങ്കിൽ (!packed()) {
		bits_.തള്ളുക_back(bit);
		width_ += 1;
		തിരികെ;
	}

	എങ്കിൽ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

ശൂന്യം DriveSpec::append(DriveChunk സ്ഥിരം &chunk)
{
	hash_ = 0;
	pack();
	എങ്കിൽ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

ശൂന്യം DriveSpec::pack() സ്ഥിരം {
	എങ്കിൽ (bits_.empty())
		തിരികെ;
	std::vector<DriveBit> bits(std::move(bits_));
	വേണ്ടി (auto &bit : bits)
		എങ്കിൽ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

ശൂന്യം DriveSpec::unpack() സ്ഥിരം {
	എങ്കിൽ (chunks_.empty())
		തിരികെ;
	വേണ്ടി (auto &chunk : chunks_)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

ശൂന്യം DriveSpec::compute_width()
{
	width_ = 0;
	വേണ്ടി (auto സ്ഥിരം &chunk : chunks_)
		width_ += chunk.size();
}

ശൂന്യം DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	എങ്കിൽ (first_edges.emplace(src, dst).first->second == dst)
		തിരികെ;
	എങ്കിൽ (second_edges.emplace(src, dst).first->second == dst)
		തിരികെ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::എടുക്കുക_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	എങ്കിൽ (found_more != more_edges.end()) {
		auto result = found_more->second.എടുക്കുക();
		എങ്കിൽ (found_more->second.empty())
			more_edges.erase(found_more);
		തിരികെ result;
	}

	auto found_second = second_edges.find(src);
	എങ്കിൽ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		തിരികെ result;
	}

	auto found_first = first_edges.find(src);
	എങ്കിൽ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		തിരികെ result;
	}

	തിരികെ DriveBitId();
}

ശൂന്യം DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

തർക്കം DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	തിരികെ first_edges.എണ്ണം(src);
}

പൂർണ്ണസംഖ്യ DriverMap::DriveBitGraph::എണ്ണം(DriveBitId src)
{
	എങ്കിൽ (!first_edges.എണ്ണം(src))
		തിരികെ 0;
	എങ്കിൽ (!second_edges.എണ്ണം(src))
		തിരികെ 1;
	auto found = more_edges.find(src);
	എങ്കിൽ (found == more_edges.end())
		തിരികെ 2;
	തിരികെ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, പൂർണ്ണസംഖ്യ index)
{
	എങ്കിൽ (index == 0)
		തിരികെ first_edges.at(src);
	അല്ലെങ്കിൽ എങ്കിൽ (index == 1)
		തിരികെ second_edges.at(src);
	അല്ലെങ്കിൽ
		തിരികെ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit സ്ഥിരം &bit)
{
	തിരഞ്ഞെടുപ്പ് (bit.type())
	{
		അവസ്ഥ DriveType::NONE:
			തിരികെ BitMode::NONE;
		അവസ്ഥ DriveType::CONSTANT:
			// TODO how to handle Sx here?
			തിരികെ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		അവസ്ഥ DriveType::WIRE: {
			auto സ്ഥിരം &wire = bit.wire();
			തർക്കം driver = wire.wire->port_നിവേശം;
			തർക്കം driven = wire.wire->port_output;

			എങ്കിൽ (driver && !driven)
				തിരികെ BitMode::DRIVER;
			അല്ലെങ്കിൽ എങ്കിൽ (driven && !driver)
				തിരികെ BitMode::DRIVEN;
			അല്ലെങ്കിൽ എങ്കിൽ (driver && driven)
				തിരികെ BitMode::TRISTATE;
			അല്ലെങ്കിൽ
				തിരികെ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		അവസ്ഥ DriveType::PORT: {
			auto സ്ഥിരം &port = bit.port();
			തർക്കം driver = celltypes.cell_output(port.cell->type, port.port);
			തർക്കം driven = celltypes.cell_നിവേശം(port.cell->type, port.port);
			എങ്കിൽ (driver && !driven)
				തിരികെ BitMode::DRIVER;
			അല്ലെങ്കിൽ എങ്കിൽ (driven && !driver)
				തിരികെ BitMode::DRIVEN_UNIQUE;
			അല്ലെങ്കിൽ
				തിരികെ BitMode::TRISTATE;
		}
		അവസ്ഥ DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		സ്ഥിരസ്ഥിതി:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit സ്ഥിരം &bit)
{
	തിരഞ്ഞെടുപ്പ് (bit.type())
	{
		അവസ്ഥ DriveType::NONE:
			തിരികെ -1;
		അവസ്ഥ DriveType::CONSTANT:
			തിരികെ (പൂർണ്ണസംഖ്യ)bit.constant();
		അവസ്ഥ DriveType::WIRE: {
			auto സ്ഥിരം &wire_bit = bit.wire();
			പൂർണ്ണസംഖ്യ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			എങ്കിൽ (insertion.second) {
				എങ്കിൽ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} അല്ലെങ്കിൽ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			തിരികെ insertion.first->second.id + wire_bit.offset;
		}
		അവസ്ഥ DriveType::PORT: {
			auto സ്ഥിരം &port_bit = bit.port();
			auto താക്കോൽ = std::make_pair(port_bit.cell, port_bit.port);
			പൂർണ്ണസംഖ്യ offset = next_offset;
			auto insertion = port_offsets.emplace(താക്കോൽ, offset);
			എങ്കിൽ (insertion.second) {
				പൂർണ്ണസംഖ്യ width = port_bit.cell->connections().at(port_bit.port).size();
				എങ്കിൽ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} അല്ലെങ്കിൽ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			തിരികെ insertion.first->second.id + port_bit.offset;
		}
		സ്ഥിരസ്ഥിതി:
			log_assert(അസത്യം && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	എങ്കിൽ (found_isolated != isolated_drive_bits.end())
		തിരികെ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	എങ്കിൽ (found == drive_bits.begin()) {
		തിരികെ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	എങ്കിൽ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} അല്ലെങ്കിൽ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	തിരികെ result;
}

ശൂന്യം DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	എങ്കിൽ (driven_id == driver_id)
		തിരികെ;

	same_driver.merge(driven_id, driver_id);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, end = connected_drivers.എണ്ണം(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, end = connected_undirected.എണ്ണം(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

ശൂന്യം DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

ശൂന്യം DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

ശൂന്യം DriverMap::add(Module *module)
{
	വേണ്ടി (auto സ്ഥിരം &conn : module->connections())
		add(conn.first, conn.second);

	വേണ്ടി (auto cell : module->cells())
		വേണ്ടി (auto സ്ഥിരം &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
ശൂന്യം DriverMap::add(DriveBit സ്ഥിരം &a, DriveBit സ്ഥിരം &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	എങ്കിൽ (a_id == b_id)
		തിരികെ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	എങ്കിൽ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	അല്ലെങ്കിൽ എങ്കിൽ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	അല്ലെങ്കിൽ എങ്കിൽ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	അല്ലെങ്കിൽ എങ്കിൽ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	അല്ലെങ്കിൽ എങ്കിൽ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	അല്ലെങ്കിൽ എങ്കിൽ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	അല്ലെങ്കിൽ
		connect_undirected(a_id, b_id);

	തിരികെ;
}

// Specialized version that avoids unpacking
ശൂന്യം DriverMap::add(SigSpec സ്ഥിരം &a, SigSpec സ്ഥിരം &b)
{
	log_assert(a.size() == b.size());
	auto സ്ഥിരം &a_chunks = a.chunks();
	auto സ്ഥിരം &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	പൂർണ്ണസംഖ്യ a_offset = 0;

	auto b_chunk = b_chunks.begin();
	പൂർണ്ണസംഖ്യ b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ആകുമ്പോൾ (a_chunk != a_end) {
		പൂർണ്ണസംഖ്യ a_width = a_chunk->width - a_offset;
		എങ്കിൽ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			തുടരുക;
		}
		പൂർണ്ണസംഖ്യ b_width = b_chunk->width - b_offset;
		എങ്കിൽ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			തുടരുക;
		}
		പൂർണ്ണസംഖ്യ width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk സ്ഥിരം &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk സ്ഥിരം &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

ശൂന്യം DriverMap::add_port(Cell *cell, IdString port, SigSpec സ്ഥിരം &b)
{
	പൂർണ്ണസംഖ്യ offset = 0;
	വേണ്ടി (auto സ്ഥിരം &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

ശൂന്യം DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, പൂർണ്ണസംഖ്യ> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	വേണ്ടി (പൂർണ്ണസംഖ്യ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		എങ്കിൽ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		എങ്കിൽ (connected_drivers.contains(current))
			drivers.emplace(current);

		പൂർണ്ണസംഖ്യ undirected_driver_എണ്ണം = connected_undirected.എണ്ണം(current);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != undirected_driver_എണ്ണം; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	എങ്കിൽ (drivers.empty())
		വേണ്ടി (auto seen_id : seen)
			drivers.emplace(seen_id);

	വേണ്ടി (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		വേണ്ടി (പൂർണ്ണസംഖ്യ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			പൂർണ്ണസംഖ്യ undirected_driver_എണ്ണം = connected_undirected.എണ്ണം(current);

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != undirected_driver_എണ്ണം; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				എങ്കിൽ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	വേണ്ടി (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit സ്ഥിരം &bit)
{
	എങ്കിൽ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		തിരികെ bit;
	എങ്കിൽ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		വേണ്ടി (auto സ്ഥിരം &inner : bit.multiple().multiple())
			result.merge((*ഇത്)(inner));
		തിരികെ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	എങ്കിൽ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		തിരികെ bit_repr;

	പൂർണ്ണസംഖ്യ implicit_driver_എണ്ണം = connected_drivers.എണ്ണം(bit_repr_id);
	എങ്കിൽ (connected_undirected.contains(bit_repr_id) && !oriented_present.എണ്ണം(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	എങ്കിൽ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != implicit_driver_എണ്ണം; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	പൂർണ്ണസംഖ്യ oriented_driver_എണ്ണം = connected_oriented.എണ്ണം(bit_repr_id);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != oriented_driver_എണ്ണം; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	തിരികെ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, width = spec.size(); i != width; ++i)
		result.append((*ഇത്)(spec[i]));

	തിരികെ result;
}

std::string log_signal(DriveChunkWire സ്ഥിരം &chunk)
{
	std::string id = chunk.wire->name.unescape();
	എങ്കിൽ (chunk.is_whole())
		തിരികെ id;
	എങ്കിൽ (chunk.width == 1)
		തിരികെ stringf("%s [%d]", id, chunk.offset);
	തിരികെ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort സ്ഥിരം &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	എങ്കിൽ (chunk.is_whole())
		തിരികെ stringf("%s <%s>", cell_id, port_id);
	എങ്കിൽ (chunk.width == 1)
		തിരികെ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	തിരികെ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker സ്ഥിരം &chunk)
{
	എങ്കിൽ (chunk.width == 1)
		തിരികെ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	തിരികെ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk സ്ഥിരം &chunk)
{
	തിരഞ്ഞെടുപ്പ് (chunk.type())
	{
		അവസ്ഥ DriveType::NONE:
			തിരികെ stringf("<none x%d>", chunk.size());
		അവസ്ഥ DriveType::CONSTANT:
			തിരികെ log_const(chunk.constant());
		അവസ്ഥ DriveType::WIRE:
			തിരികെ log_signal(chunk.wire());
		അവസ്ഥ DriveType::PORT:
			തിരികെ log_signal(chunk.port());
		അവസ്ഥ DriveType::MARKER:
			തിരികെ log_signal(chunk.marker());
		അവസ്ഥ DriveType::MULTIPLE: {
			std::string str = "<multiple";
			സ്ഥിരം അക്ഷരം *sep = " ";
			വേണ്ടി (auto സ്ഥിരം &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			തിരികെ str;
		}
		സ്ഥിരസ്ഥിതി:
			log_abort();
	}
}

std::string log_signal(DriveSpec സ്ഥിരം &spec)
{
	auto &chunks = spec.chunks();
	എങ്കിൽ (chunks.empty())
		തിരികെ "{}";
	എങ്കിൽ (chunks.size() == 1)
		തിരികെ log_signal(chunks[0]);

	std::string str;
	സ്ഥിരം അക്ഷരം *sep = "{ ";

	വേണ്ടി (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	തിരികെ str;
}

YOSYS_NAMESPACE_END
