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

#include "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit const &bit)
{
	ᱡᱩᱫᱤ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	ᱵᱟᱝ
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ᱡᱩᱫᱤ (other.type_ == DriveType::NONE)
		ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (type_ == DriveType::NONE) {
		*this = other;
		ᱨᱩᱣᱟᱹᱲ;
	}
	ᱡᱩᱫᱤ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ᱡᱩᱫᱤ (single.type() == DriveType::NONE)
		ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ᱨᱩᱣᱟᱹᱲ;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ᱡᱩᱫᱤ (single.type() == DriveType::NONE)
		ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ᱨᱩᱣᱟᱹᱲ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](puur_naa_nka i) const
{
	DriveBitMultiple result;
	for (auto const &single : multiple_)
		result.merge(single[i]);
	ᱨᱩᱣᱟᱹᱲ result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ᱨᱩᱣᱟᱹᱲ bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ᱡᱩᱫᱤ (!can_append(bit))
		ᱨᱩᱣᱟᱹᱲ false;
	width += 1;
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ᱡᱩᱫᱤ (chunk.wire != wire || chunk.offset != offset + width)
		ᱨᱩᱣᱟᱹᱲ false;
	width += chunk.width;
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ᱨᱩᱣᱟᱹᱲ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ᱡᱩᱫᱤ (!can_append(bit))
		ᱨᱩᱣᱟᱹᱲ false;
	width += 1;
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ᱡᱩᱫᱤ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ᱨᱩᱣᱟᱹᱲ false;
	width += chunk.width;
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ᱨᱩᱣᱟᱹᱲ bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ᱡᱩᱫᱤ (!can_append(bit))
		ᱨᱩᱣᱟᱹᱲ false;
	width += 1;
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ᱡᱩᱫᱤ (chunk.marker != marker || chunk.offset != offset + width)
		ᱨᱩᱣᱟᱹᱲ false;
	width += chunk.width;
	ᱨᱩᱣᱟᱹᱲ true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ᱡᱩᱫᱤ (bit.multiple().size() != multiple_.size())
		ᱨᱩᱣᱟᱹᱲ false;

	puur_naa_nka const_drivers = 0;
	for (DriveChunk const &single : multiple_)
		ᱡᱩᱫᱤ (single.is_constant())
			const_drivers += 1;

	ᱡᱩᱫᱤ (const_drivers > 1)
		ᱨᱩᱣᱟᱹᱲ false;

	for (DriveBit const &single : bit.multiple())
		ᱡᱩᱫᱤ (single.is_constant())
			const_drivers -= 1;

	ᱡᱩᱫᱤ (const_drivers != 0)
		ᱨᱩᱣᱟᱹᱲ false;

	for (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ᱡᱩᱫᱤ (!bit.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ᱡᱩᱫᱤ (!bit.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ᱡᱩᱫᱤ (!bit.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			default:
				ᱨᱩᱣᱟᱹᱲ false;
		}
	}

	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ᱡᱩᱫᱤ (chunk.multiple().size() != multiple_.size())
		ᱨᱩᱣᱟᱹᱲ false;

	puur_naa_nka const_drivers = 0;
	for (DriveChunk const &single : multiple_)
		ᱡᱩᱫᱤ (single.is_constant())
			const_drivers += 1;

	ᱡᱩᱫᱤ (const_drivers > 1)
		ᱨᱩᱣᱟᱹᱲ false;

	for (DriveChunk const &single : chunk.multiple())
		ᱡᱩᱫᱤ (single.is_constant())
			const_drivers -= 1;

	ᱡᱩᱫᱤ (const_drivers != 0)
		ᱨᱩᱣᱟᱹᱲ false;

	for (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ᱡᱩᱫᱤ (!chunk.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ᱡᱩᱫᱤ (!chunk.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ᱡᱩᱫᱤ (!chunk.multiple().count(next))
					ᱨᱩᱣᱟᱹᱲ false;
			} bha_nga;
			default:
				ᱨᱩᱣᱟᱹᱲ false;
		}
	}

	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ᱡᱩᱫᱤ (!can_append(bit))
		ᱨᱩᱣᱟᱹᱲ false;
	width_ += 1;
	State constant;

	for (DriveBit const &single : bit.multiple())
		ᱡᱩᱫᱤ (single.is_constant())
			constant = single.constant();

	for (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} bha_nga;
			pak_sa DriveType::WIRE: {
				single.wire().width += 1;
			} bha_nga;
			pak_sa DriveType::PORT: {
				single.port().width += 1;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				single.marker().width += 1;
			} bha_nga;
			default:
				log_abort();
		}
	}
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ᱡᱩᱫᱤ (!can_append(chunk))
		ᱨᱩᱣᱟᱹᱲ false;
	puur_naa_nka width = chunk.size();
	width_ += width;
	Const constant;

	for (DriveChunk const &single : chunk.multiple())
		ᱡᱩᱫᱤ (single.is_constant())
			constant = single.constant();

	for (DriveChunk &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
				single.constant().append(constant);
			} bha_nga;
			pak_sa DriveType::WIRE: {
				single.wire().width += width;
			} bha_nga;
			pak_sa DriveType::PORT: {
				single.port().width += width;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				single.marker().width += width;
			} bha_nga;
			default:
				log_abort();
		}
	}
	ᱨᱩᱣᱟᱹᱲ true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ᱡᱩᱫᱤ (size() == 0)
		ᱨᱩᱣᱟᱹᱲ true;
	ᱡᱩᱫᱤ (bit.type() != type_)
		ᱨᱩᱣᱟᱹᱲ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::CONSTANT:
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::WIRE:
			ᱨᱩᱣᱟᱹᱲ wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ᱨᱩᱣᱟᱹᱲ port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ᱨᱩᱣᱟᱹᱲ multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ᱡᱩᱫᱤ (size() == 0)
		*this = bit;
	ᱡᱩᱫᱤ (bit.type() != type_)
		ᱨᱩᱣᱟᱹᱲ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::WIRE:
			ᱨᱩᱣᱟᱹᱲ wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ᱨᱩᱣᱟᱹᱲ port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ᱨᱩᱣᱟᱹᱲ multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ᱡᱩᱫᱤ (size() == 0)
		*this = chunk;
	ᱡᱩᱫᱤ (chunk.type_ != type_)
		ᱨᱩᱣᱟᱹᱲ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ᱨᱩᱣᱟᱹᱲ true;
		pak_sa DriveType::WIRE:
			ᱨᱩᱣᱟᱹᱲ wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ᱨᱩᱣᱟᱹᱲ port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ᱨᱩᱣᱟᱹᱲ marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ᱨᱩᱣᱟᱹᱲ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ᱡᱩᱫᱤ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ᱨᱩᱣᱟᱹᱲ;
	}

	ᱡᱩᱫᱤ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ᱡᱩᱫᱤ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ᱡᱩᱫᱤ (bits_.empty())
		ᱨᱩᱣᱟᱹᱲ;
	std::vector<DriveBit> bits(std::move(bits_));
	for (auto &bit : bits)
		ᱡᱩᱫᱤ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ᱡᱩᱫᱤ (chunks_.empty())
		ᱨᱩᱣᱟᱹᱲ;
	for (auto &chunk : chunks_)
	{
		for (puur_naa_nka i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	for (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ᱡᱩᱫᱤ (first_edges.emplace(src, dst).first->second == dst)
		ᱨᱩᱣᱟᱹᱲ;
	ᱡᱩᱫᱤ (second_edges.emplace(src, dst).first->second == dst)
		ᱨᱩᱣᱟᱹᱲ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ᱡᱩᱫᱤ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ᱡᱩᱫᱤ (found_more->second.empty())
			more_edges.erase(found_more);
		ᱨᱩᱣᱟᱹᱲ result;
	}

	auto found_second = second_edges.find(src);
	ᱡᱩᱫᱤ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ᱨᱩᱣᱟᱹᱲ result;
	}

	auto found_first = first_edges.find(src);
	ᱡᱩᱫᱤ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ᱨᱩᱣᱟᱹᱲ result;
	}

	ᱨᱩᱣᱟᱹᱲ DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ᱨᱩᱣᱟᱹᱲ first_edges.count(src);
}

puur_naa_nka DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ᱡᱩᱫᱤ (!first_edges.count(src))
		ᱨᱩᱣᱟᱹᱲ 0;
	ᱡᱩᱫᱤ (!second_edges.count(src))
		ᱨᱩᱣᱟᱹᱲ 1;
	auto found = more_edges.find(src);
	ᱡᱩᱫᱤ (found == more_edges.end())
		ᱨᱩᱣᱟᱹᱲ 2;
	ᱨᱩᱣᱟᱹᱲ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, puur_naa_nka index)
{
	ᱡᱩᱫᱤ (index == 0)
		ᱨᱩᱣᱟᱹᱲ first_edges.at(src);
	ᱵᱟᱝ ᱡᱩᱫᱤ (index == 1)
		ᱨᱩᱣᱟᱹᱲ second_edges.at(src);
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ᱨᱩᱣᱟᱹᱲ BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ᱨᱩᱣᱟᱹᱲ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ᱡᱩᱫᱤ (driver && !driven)
				ᱨᱩᱣᱟᱹᱲ BitMode::DRIVER;
			ᱵᱟᱝ ᱡᱩᱫᱤ (driven && !driver)
				ᱨᱩᱣᱟᱹᱲ BitMode::DRIVEN;
			ᱵᱟᱝ ᱡᱩᱫᱤ (driver && driven)
				ᱨᱩᱣᱟᱹᱲ BitMode::TRISTATE;
			ᱵᱟᱝ
				ᱨᱩᱣᱟᱹᱲ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ᱡᱩᱫᱤ (driver && !driven)
				ᱨᱩᱣᱟᱹᱲ BitMode::DRIVER;
			ᱵᱟᱝ ᱡᱩᱫᱤ (driven && !driver)
				ᱨᱩᱣᱟᱹᱲ BitMode::DRIVEN_UNIQUE;
			ᱵᱟᱝ
				ᱨᱩᱣᱟᱹᱲ BitMode::TRISTATE;
		}
		pak_sa DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		default:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ᱨᱩᱣᱟᱹᱲ -1;
		pak_sa DriveType::CONSTANT:
			ᱨᱩᱣᱟᱹᱲ (puur_naa_nka)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			puur_naa_nka offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ᱡᱩᱫᱤ (insertion.second) {
				ᱡᱩᱫᱤ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ᱵᱟᱝ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ᱨᱩᱣᱟᱹᱲ insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			puur_naa_nka offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ᱡᱩᱫᱤ (insertion.second) {
				puur_naa_nka width = port_bit.cell->connections().at(port_bit.port).size();
				ᱡᱩᱫᱤ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ᱵᱟᱝ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ᱨᱩᱣᱟᱹᱲ insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ᱡᱩᱫᱤ (found_isolated != isolated_drive_bits.end())
		ᱨᱩᱣᱟᱹᱲ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ᱡᱩᱫᱤ (found == drive_bits.begin()) {
		ᱨᱩᱣᱟᱹᱲ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ᱡᱩᱫᱤ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ᱵᱟᱝ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ᱨᱩᱣᱟᱹᱲ result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ᱡᱩᱫᱤ (driven_id == driver_id)
		ᱨᱩᱣᱟᱹᱲ;

	same_driver.merge(driven_id, driver_id);

	for (puur_naa_nka i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	for (puur_naa_nka i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

void DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

void DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

void DriverMap::add(Module *module)
{
	for (auto const &conn : module->connections())
		add(conn.first, conn.second);

	for (auto cell : module->cells())
		for (auto const &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
void DriverMap::add(DriveBit const &a, DriveBit const &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	ᱡᱩᱫᱤ (a_id == b_id)
		ᱨᱩᱣᱟᱹᱲ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ᱡᱩᱫᱤ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ᱵᱟᱝ ᱡᱩᱫᱤ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ᱵᱟᱝ ᱡᱩᱫᱤ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ᱵᱟᱝ ᱡᱩᱫᱤ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ᱵᱟᱝ ᱡᱩᱫᱤ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ᱵᱟᱝ ᱡᱩᱫᱤ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ᱵᱟᱝ
		connect_undirected(a_id, b_id);

	ᱨᱩᱣᱟᱹᱲ;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	puur_naa_nka a_offset = 0;

	auto b_chunk = b_chunks.begin();
	puur_naa_nka b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ᱡᱚᱠᱷᱚᱱ (a_chunk != a_end) {
		puur_naa_nka a_width = a_chunk->width - a_offset;
		ᱡᱩᱫᱤ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		puur_naa_nka b_width = b_chunk->width - b_offset;
		ᱡᱩᱫᱤ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		puur_naa_nka width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk const &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk const &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

void DriverMap::add_port(Cell *cell, IdString port, SigSpec const &b)
{
	puur_naa_nka offset = 0;
	for (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, puur_naa_nka> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	for (puur_naa_nka pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ᱡᱩᱫᱤ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ᱡᱩᱫᱤ (connected_drivers.contains(current))
			drivers.emplace(current);

		puur_naa_nka undirected_driver_count = connected_undirected.count(current);

		for (puur_naa_nka i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ᱡᱩᱫᱤ (drivers.empty())
		for (auto seen_id : seen)
			drivers.emplace(seen_id);

	for (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		for (puur_naa_nka pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			puur_naa_nka undirected_driver_count = connected_undirected.count(current);

			for (puur_naa_nka i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ᱡᱩᱫᱤ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	for (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ᱡᱩᱫᱤ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ᱨᱩᱣᱟᱹᱲ bit;
	ᱡᱩᱫᱤ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		for (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ᱨᱩᱣᱟᱹᱲ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ᱡᱩᱫᱤ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ᱨᱩᱣᱟᱹᱲ bit_repr;

	puur_naa_nka implicit_driver_count = connected_drivers.count(bit_repr_id);
	ᱡᱩᱫᱤ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ᱡᱩᱫᱤ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	for (puur_naa_nka i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	puur_naa_nka oriented_driver_count = connected_oriented.count(bit_repr_id);
	for (puur_naa_nka i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ᱨᱩᱣᱟᱹᱲ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	for (puur_naa_nka i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ᱨᱩᱣᱟᱹᱲ result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ᱡᱩᱫᱤ (chunk.is_whole())
		ᱨᱩᱣᱟᱹᱲ id;
	ᱡᱩᱫᱤ (chunk.width == 1)
		ᱨᱩᱣᱟᱹᱲ stringf("%s [%d]", id, chunk.offset);
	ᱨᱩᱣᱟᱹᱲ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ᱡᱩᱫᱤ (chunk.is_whole())
		ᱨᱩᱣᱟᱹᱲ stringf("%s <%s>", cell_id, port_id);
	ᱡᱩᱫᱤ (chunk.width == 1)
		ᱨᱩᱣᱟᱹᱲ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ᱨᱩᱣᱟᱹᱲ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ᱡᱩᱫᱤ (chunk.width == 1)
		ᱨᱩᱣᱟᱹᱲ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ᱨᱩᱣᱟᱹᱲ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ᱨᱩᱣᱟᱹᱲ stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ᱨᱩᱣᱟᱹᱲ log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ᱨᱩᱣᱟᱹᱲ log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ᱨᱩᱣᱟᱹᱲ log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ᱨᱩᱣᱟᱹᱲ log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			for (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ᱨᱩᱣᱟᱹᱲ str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ᱡᱩᱫᱤ (chunks.empty())
		ᱨᱩᱣᱟᱹᱲ "{}";
	ᱡᱩᱫᱤ (chunks.size() == 1)
		ᱨᱩᱣᱟᱹᱲ log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	for (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ᱨᱩᱣᱟᱹᱲ str;
}

YOSYS_NAMESPACE_END
