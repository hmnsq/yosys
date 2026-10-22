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
	ꦪꦼꦤ꧀ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	ꦭꦶꦪ
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ꦪꦼꦤ꧀ (other.type_ == DriveType::NONE)
		ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (type_ == DriveType::NONE) {
		*this = other;
		ꦮꦭꦶ;
	}
	ꦪꦼꦤ꧀ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ꦪꦼꦤ꧀ (single.type() == DriveType::NONE)
		ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ꦮꦭꦶ;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ꦪꦼꦤ꧀ (single.type() == DriveType::NONE)
		ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ꦮꦭꦶ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](puur_naa_nka i) const
{
	DriveBitMultiple result;
	ꦏꦁꦒꦺꦴ (auto const &single : multiple_)
		result.merge(single[i]);
	ꦮꦭꦶ result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ꦮꦭꦶ bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ꦪꦼꦤ꧀ (!can_append(bit))
		ꦮꦭꦶ false;
	width += 1;
	ꦮꦭꦶ true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ꦪꦼꦤ꧀ (chunk.wire != wire || chunk.offset != offset + width)
		ꦮꦭꦶ false;
	width += chunk.width;
	ꦮꦭꦶ true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ꦮꦭꦶ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ꦪꦼꦤ꧀ (!can_append(bit))
		ꦮꦭꦶ false;
	width += 1;
	ꦮꦭꦶ true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ꦪꦼꦤ꧀ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ꦮꦭꦶ false;
	width += chunk.width;
	ꦮꦭꦶ true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ꦮꦭꦶ bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ꦪꦼꦤ꧀ (!can_append(bit))
		ꦮꦭꦶ false;
	width += 1;
	ꦮꦭꦶ true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ꦪꦼꦤ꧀ (chunk.marker != marker || chunk.offset != offset + width)
		ꦮꦭꦶ false;
	width += chunk.width;
	ꦮꦭꦶ true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ꦪꦼꦤ꧀ (bit.multiple().size() != multiple_.size())
		ꦮꦭꦶ false;

	puur_naa_nka const_drivers = 0;
	ꦏꦁꦒꦺꦴ (DriveChunk const &single : multiple_)
		ꦪꦼꦤ꧀ (single.is_constant())
			const_drivers += 1;

	ꦪꦼꦤ꧀ (const_drivers > 1)
		ꦮꦭꦶ false;

	ꦏꦁꦒꦺꦴ (DriveBit const &single : bit.multiple())
		ꦪꦼꦤ꧀ (single.is_constant())
			const_drivers -= 1;

	ꦪꦼꦤ꧀ (const_drivers != 0)
		ꦮꦭꦶ false;

	ꦏꦁꦒꦺꦴ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ꦪꦼꦤ꧀ (!bit.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ꦪꦼꦤ꧀ (!bit.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ꦪꦼꦤ꧀ (!bit.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			default:
				ꦮꦭꦶ false;
		}
	}

	ꦮꦭꦶ true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ꦪꦼꦤ꧀ (chunk.multiple().size() != multiple_.size())
		ꦮꦭꦶ false;

	puur_naa_nka const_drivers = 0;
	ꦏꦁꦒꦺꦴ (DriveChunk const &single : multiple_)
		ꦪꦼꦤ꧀ (single.is_constant())
			const_drivers += 1;

	ꦪꦼꦤ꧀ (const_drivers > 1)
		ꦮꦭꦶ false;

	ꦏꦁꦒꦺꦴ (DriveChunk const &single : chunk.multiple())
		ꦪꦼꦤ꧀ (single.is_constant())
			const_drivers -= 1;

	ꦪꦼꦤ꧀ (const_drivers != 0)
		ꦮꦭꦶ false;

	ꦏꦁꦒꦺꦴ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ꦪꦼꦤ꧀ (!chunk.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ꦪꦼꦤ꧀ (!chunk.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ꦪꦼꦤ꧀ (!chunk.multiple().count(next))
					ꦮꦭꦶ false;
			} bha_nga;
			default:
				ꦮꦭꦶ false;
		}
	}

	ꦮꦭꦶ true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ꦪꦼꦤ꧀ (!can_append(bit))
		ꦮꦭꦶ false;
	width_ += 1;
	State constant;

	ꦏꦁꦒꦺꦴ (DriveBit const &single : bit.multiple())
		ꦪꦼꦤ꧀ (single.is_constant())
			constant = single.constant();

	ꦏꦁꦒꦺꦴ (DriveChunk &single : multiple_)
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
	ꦮꦭꦶ true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ꦪꦼꦤ꧀ (!can_append(chunk))
		ꦮꦭꦶ false;
	puur_naa_nka width = chunk.size();
	width_ += width;
	Const constant;

	ꦏꦁꦒꦺꦴ (DriveChunk const &single : chunk.multiple())
		ꦪꦼꦤ꧀ (single.is_constant())
			constant = single.constant();

	ꦏꦁꦒꦺꦴ (DriveChunk &single : multiple_)
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
	ꦮꦭꦶ true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ꦪꦼꦤ꧀ (size() == 0)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (bit.type() != type_)
		ꦮꦭꦶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ꦮꦭꦶ true;
		pak_sa DriveType::CONSTANT:
			ꦮꦭꦶ true;
		pak_sa DriveType::WIRE:
			ꦮꦭꦶ wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ꦮꦭꦶ port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ꦮꦭꦶ multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ꦪꦼꦤ꧀ (size() == 0)
		*this = bit;
	ꦪꦼꦤ꧀ (bit.type() != type_)
		ꦮꦭꦶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ꦮꦭꦶ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ꦮꦭꦶ true;
		pak_sa DriveType::WIRE:
			ꦮꦭꦶ wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ꦮꦭꦶ port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ꦮꦭꦶ multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ꦪꦼꦤ꧀ (size() == 0)
		*this = chunk;
	ꦪꦼꦤ꧀ (chunk.type_ != type_)
		ꦮꦭꦶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ꦮꦭꦶ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ꦮꦭꦶ true;
		pak_sa DriveType::WIRE:
			ꦮꦭꦶ wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ꦮꦭꦶ port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ꦮꦭꦶ marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ꦮꦭꦶ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ꦪꦼꦤ꧀ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ꦪꦼꦤ꧀ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ꦪꦼꦤ꧀ (bits_.empty())
		ꦮꦭꦶ;
	std::vector<DriveBit> bits(std::move(bits_));
	ꦏꦁꦒꦺꦴ (auto &bit : bits)
		ꦪꦼꦤ꧀ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ꦪꦼꦤ꧀ (chunks_.empty())
		ꦮꦭꦶ;
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks_)
	{
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	ꦏꦁꦒꦺꦴ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ꦪꦼꦤ꧀ (first_edges.emplace(src, dst).first->second == dst)
		ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (second_edges.emplace(src, dst).first->second == dst)
		ꦮꦭꦶ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ꦪꦼꦤ꧀ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ꦪꦼꦤ꧀ (found_more->second.empty())
			more_edges.erase(found_more);
		ꦮꦭꦶ result;
	}

	auto found_second = second_edges.find(src);
	ꦪꦼꦤ꧀ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ꦮꦭꦶ result;
	}

	auto found_first = first_edges.find(src);
	ꦪꦼꦤ꧀ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ꦮꦭꦶ result;
	}

	ꦮꦭꦶ DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ꦮꦭꦶ first_edges.count(src);
}

puur_naa_nka DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ꦪꦼꦤ꧀ (!first_edges.count(src))
		ꦮꦭꦶ 0;
	ꦪꦼꦤ꧀ (!second_edges.count(src))
		ꦮꦭꦶ 1;
	auto found = more_edges.find(src);
	ꦪꦼꦤ꧀ (found == more_edges.end())
		ꦮꦭꦶ 2;
	ꦮꦭꦶ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, puur_naa_nka index)
{
	ꦪꦼꦤ꧀ (index == 0)
		ꦮꦭꦶ first_edges.at(src);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (index == 1)
		ꦮꦭꦶ second_edges.at(src);
	ꦭꦶꦪ
		ꦮꦭꦶ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ꦮꦭꦶ BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ꦮꦭꦶ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ꦪꦼꦤ꧀ (driver && !driven)
				ꦮꦭꦶ BitMode::DRIVER;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (driven && !driver)
				ꦮꦭꦶ BitMode::DRIVEN;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (driver && driven)
				ꦮꦭꦶ BitMode::TRISTATE;
			ꦭꦶꦪ
				ꦮꦭꦶ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ꦪꦼꦤ꧀ (driver && !driven)
				ꦮꦭꦶ BitMode::DRIVER;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (driven && !driver)
				ꦮꦭꦶ BitMode::DRIVEN_UNIQUE;
			ꦭꦶꦪ
				ꦮꦭꦶ BitMode::TRISTATE;
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
			ꦮꦭꦶ -1;
		pak_sa DriveType::CONSTANT:
			ꦮꦭꦶ (puur_naa_nka)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			puur_naa_nka offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ꦪꦼꦤ꧀ (insertion.second) {
				ꦪꦼꦤ꧀ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ꦭꦶꦪ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ꦮꦭꦶ insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			puur_naa_nka offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ꦪꦼꦤ꧀ (insertion.second) {
				puur_naa_nka width = port_bit.cell->connections().at(port_bit.port).size();
				ꦪꦼꦤ꧀ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ꦭꦶꦪ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ꦮꦭꦶ insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ꦪꦼꦤ꧀ (found_isolated != isolated_drive_bits.end())
		ꦮꦭꦶ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ꦪꦼꦤ꧀ (found == drive_bits.begin()) {
		ꦮꦭꦶ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ꦪꦼꦤ꧀ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ꦭꦶꦪ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ꦮꦭꦶ result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ꦪꦼꦤ꧀ (driven_id == driver_id)
		ꦮꦭꦶ;

	same_driver.merge(driven_id, driver_id);

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	ꦏꦁꦒꦺꦴ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	ꦏꦁꦒꦺꦴ (auto cell : module->cells())
		ꦏꦁꦒꦺꦴ (auto const &conn : cell->connections())
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

	ꦪꦼꦤ꧀ (a_id == b_id)
		ꦮꦭꦶ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ꦪꦼꦤ꧀ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ꦭꦶꦪ ꦪꦼꦤ꧀ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ꦭꦶꦪ ꦪꦼꦤ꧀ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ꦭꦶꦪ
		connect_undirected(a_id, b_id);

	ꦮꦭꦶ;
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
	ꦱꦼꦢꦼꦭ (a_chunk != a_end) {
		puur_naa_nka a_width = a_chunk->width - a_offset;
		ꦪꦼꦤ꧀ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		puur_naa_nka b_width = b_chunk->width - b_offset;
		ꦪꦼꦤ꧀ (b_width == 0) {
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
	ꦏꦁꦒꦺꦴ (auto const &chunk : b.chunks()) {
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

	ꦏꦁꦒꦺꦴ (puur_naa_nka pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ꦪꦼꦤ꧀ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ꦪꦼꦤ꧀ (connected_drivers.contains(current))
			drivers.emplace(current);

		puur_naa_nka undirected_driver_count = connected_undirected.count(current);

		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ꦪꦼꦤ꧀ (drivers.empty())
		ꦏꦁꦒꦺꦴ (auto seen_id : seen)
			drivers.emplace(seen_id);

	ꦏꦁꦒꦺꦴ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		ꦏꦁꦒꦺꦴ (puur_naa_nka pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			puur_naa_nka undirected_driver_count = connected_undirected.count(current);

			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ꦪꦼꦤ꧀ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	ꦏꦁꦒꦺꦴ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ꦪꦼꦤ꧀ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ꦮꦭꦶ bit;
	ꦪꦼꦤ꧀ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		ꦏꦁꦒꦺꦴ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ꦮꦭꦶ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ꦪꦼꦤ꧀ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ꦮꦭꦶ bit_repr;

	puur_naa_nka implicit_driver_count = connected_drivers.count(bit_repr_id);
	ꦪꦼꦤ꧀ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ꦪꦼꦤ꧀ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	puur_naa_nka oriented_driver_count = connected_oriented.count(bit_repr_id);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ꦮꦭꦶ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ꦮꦭꦶ result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ꦪꦼꦤ꧀ (chunk.is_whole())
		ꦮꦭꦶ id;
	ꦪꦼꦤ꧀ (chunk.width == 1)
		ꦮꦭꦶ stringf("%s [%d]", id, chunk.offset);
	ꦮꦭꦶ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ꦪꦼꦤ꧀ (chunk.is_whole())
		ꦮꦭꦶ stringf("%s <%s>", cell_id, port_id);
	ꦪꦼꦤ꧀ (chunk.width == 1)
		ꦮꦭꦶ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ꦮꦭꦶ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ꦪꦼꦤ꧀ (chunk.width == 1)
		ꦮꦭꦶ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ꦮꦭꦶ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ꦮꦭꦶ stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ꦮꦭꦶ log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ꦮꦭꦶ log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ꦮꦭꦶ log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ꦮꦭꦶ log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			ꦏꦁꦒꦺꦴ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ꦮꦭꦶ str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ꦪꦼꦤ꧀ (chunks.empty())
		ꦮꦭꦶ "{}";
	ꦪꦼꦤ꧀ (chunks.size() == 1)
		ꦮꦭꦶ log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	ꦏꦁꦒꦺꦴ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ꦮꦭꦶ str;
}

YOSYS_NAMESPACE_END
