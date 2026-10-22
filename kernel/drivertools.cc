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
	ᬬᬾᬦ᭄ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	ᬮᬶᬬ
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ᬬᬾᬦ᭄ (other.type_ == DriveType::NONE)
		ᬯᬮᬶ;
	ᬬᬾᬦ᭄ (type_ == DriveType::NONE) {
		*this = other;
		ᬯᬮᬶ;
	}
	ᬬᬾᬦ᭄ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ᬬᬾᬦ᭄ (single.type() == DriveType::NONE)
		ᬯᬮᬶ;
	ᬬᬾᬦ᭄ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ᬯᬮᬶ;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ᬬᬾᬦ᭄ (single.type() == DriveType::NONE)
		ᬯᬮᬶ;
	ᬬᬾᬦ᭄ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ᬯᬮᬶ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](puur_naa_nka i) const
{
	DriveBitMultiple result;
	for (auto const &single : multiple_)
		result.merge(single[i]);
	ᬯᬮᬶ result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ᬯᬮᬶ bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ᬬᬾᬦ᭄ (!can_append(bit))
		ᬯᬮᬶ false;
	width += 1;
	ᬯᬮᬶ true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ᬬᬾᬦ᭄ (chunk.wire != wire || chunk.offset != offset + width)
		ᬯᬮᬶ false;
	width += chunk.width;
	ᬯᬮᬶ true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ᬯᬮᬶ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ᬬᬾᬦ᭄ (!can_append(bit))
		ᬯᬮᬶ false;
	width += 1;
	ᬯᬮᬶ true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ᬬᬾᬦ᭄ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ᬯᬮᬶ false;
	width += chunk.width;
	ᬯᬮᬶ true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ᬯᬮᬶ bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ᬬᬾᬦ᭄ (!can_append(bit))
		ᬯᬮᬶ false;
	width += 1;
	ᬯᬮᬶ true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ᬬᬾᬦ᭄ (chunk.marker != marker || chunk.offset != offset + width)
		ᬯᬮᬶ false;
	width += chunk.width;
	ᬯᬮᬶ true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ᬬᬾᬦ᭄ (bit.multiple().size() != multiple_.size())
		ᬯᬮᬶ false;

	puur_naa_nka const_drivers = 0;
	for (DriveChunk const &single : multiple_)
		ᬬᬾᬦ᭄ (single.is_constant())
			const_drivers += 1;

	ᬬᬾᬦ᭄ (const_drivers > 1)
		ᬯᬮᬶ false;

	for (DriveBit const &single : bit.multiple())
		ᬬᬾᬦ᭄ (single.is_constant())
			const_drivers -= 1;

	ᬬᬾᬦ᭄ (const_drivers != 0)
		ᬯᬮᬶ false;

	for (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ᬬᬾᬦ᭄ (!bit.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ᬬᬾᬦ᭄ (!bit.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ᬬᬾᬦ᭄ (!bit.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			default:
				ᬯᬮᬶ false;
		}
	}

	ᬯᬮᬶ true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ᬬᬾᬦ᭄ (chunk.multiple().size() != multiple_.size())
		ᬯᬮᬶ false;

	puur_naa_nka const_drivers = 0;
	for (DriveChunk const &single : multiple_)
		ᬬᬾᬦ᭄ (single.is_constant())
			const_drivers += 1;

	ᬬᬾᬦ᭄ (const_drivers > 1)
		ᬯᬮᬶ false;

	for (DriveChunk const &single : chunk.multiple())
		ᬬᬾᬦ᭄ (single.is_constant())
			const_drivers -= 1;

	ᬬᬾᬦ᭄ (const_drivers != 0)
		ᬯᬮᬶ false;

	for (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ᬬᬾᬦ᭄ (!chunk.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ᬬᬾᬦ᭄ (!chunk.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ᬬᬾᬦ᭄ (!chunk.multiple().count(next))
					ᬯᬮᬶ false;
			} bha_nga;
			default:
				ᬯᬮᬶ false;
		}
	}

	ᬯᬮᬶ true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ᬬᬾᬦ᭄ (!can_append(bit))
		ᬯᬮᬶ false;
	width_ += 1;
	State constant;

	for (DriveBit const &single : bit.multiple())
		ᬬᬾᬦ᭄ (single.is_constant())
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
	ᬯᬮᬶ true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ᬬᬾᬦ᭄ (!can_append(chunk))
		ᬯᬮᬶ false;
	puur_naa_nka width = chunk.size();
	width_ += width;
	Const constant;

	for (DriveChunk const &single : chunk.multiple())
		ᬬᬾᬦ᭄ (single.is_constant())
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
	ᬯᬮᬶ true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ᬬᬾᬦ᭄ (size() == 0)
		ᬯᬮᬶ true;
	ᬬᬾᬦ᭄ (bit.type() != type_)
		ᬯᬮᬶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ᬯᬮᬶ true;
		pak_sa DriveType::CONSTANT:
			ᬯᬮᬶ true;
		pak_sa DriveType::WIRE:
			ᬯᬮᬶ wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ᬯᬮᬶ port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ᬯᬮᬶ multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ᬬᬾᬦ᭄ (size() == 0)
		*this = bit;
	ᬬᬾᬦ᭄ (bit.type() != type_)
		ᬯᬮᬶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ᬯᬮᬶ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ᬯᬮᬶ true;
		pak_sa DriveType::WIRE:
			ᬯᬮᬶ wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ᬯᬮᬶ port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ᬯᬮᬶ multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ᬬᬾᬦ᭄ (size() == 0)
		*this = chunk;
	ᬬᬾᬦ᭄ (chunk.type_ != type_)
		ᬯᬮᬶ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ᬯᬮᬶ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ᬯᬮᬶ true;
		pak_sa DriveType::WIRE:
			ᬯᬮᬶ wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ᬯᬮᬶ port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ᬯᬮᬶ marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ᬯᬮᬶ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ᬬᬾᬦ᭄ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ᬯᬮᬶ;
	}

	ᬬᬾᬦ᭄ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ᬬᬾᬦ᭄ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ᬬᬾᬦ᭄ (bits_.empty())
		ᬯᬮᬶ;
	std::vector<DriveBit> bits(std::move(bits_));
	for (auto &bit : bits)
		ᬬᬾᬦ᭄ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ᬬᬾᬦ᭄ (chunks_.empty())
		ᬯᬮᬶ;
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
	ᬬᬾᬦ᭄ (first_edges.emplace(src, dst).first->second == dst)
		ᬯᬮᬶ;
	ᬬᬾᬦ᭄ (second_edges.emplace(src, dst).first->second == dst)
		ᬯᬮᬶ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ᬬᬾᬦ᭄ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ᬬᬾᬦ᭄ (found_more->second.empty())
			more_edges.erase(found_more);
		ᬯᬮᬶ result;
	}

	auto found_second = second_edges.find(src);
	ᬬᬾᬦ᭄ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ᬯᬮᬶ result;
	}

	auto found_first = first_edges.find(src);
	ᬬᬾᬦ᭄ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ᬯᬮᬶ result;
	}

	ᬯᬮᬶ DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ᬯᬮᬶ first_edges.count(src);
}

puur_naa_nka DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ᬬᬾᬦ᭄ (!first_edges.count(src))
		ᬯᬮᬶ 0;
	ᬬᬾᬦ᭄ (!second_edges.count(src))
		ᬯᬮᬶ 1;
	auto found = more_edges.find(src);
	ᬬᬾᬦ᭄ (found == more_edges.end())
		ᬯᬮᬶ 2;
	ᬯᬮᬶ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, puur_naa_nka index)
{
	ᬬᬾᬦ᭄ (index == 0)
		ᬯᬮᬶ first_edges.at(src);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (index == 1)
		ᬯᬮᬶ second_edges.at(src);
	ᬮᬶᬬ
		ᬯᬮᬶ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ᬯᬮᬶ BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ᬯᬮᬶ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ᬬᬾᬦ᭄ (driver && !driven)
				ᬯᬮᬶ BitMode::DRIVER;
			ᬮᬶᬬ ᬬᬾᬦ᭄ (driven && !driver)
				ᬯᬮᬶ BitMode::DRIVEN;
			ᬮᬶᬬ ᬬᬾᬦ᭄ (driver && driven)
				ᬯᬮᬶ BitMode::TRISTATE;
			ᬮᬶᬬ
				ᬯᬮᬶ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ᬬᬾᬦ᭄ (driver && !driven)
				ᬯᬮᬶ BitMode::DRIVER;
			ᬮᬶᬬ ᬬᬾᬦ᭄ (driven && !driver)
				ᬯᬮᬶ BitMode::DRIVEN_UNIQUE;
			ᬮᬶᬬ
				ᬯᬮᬶ BitMode::TRISTATE;
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
			ᬯᬮᬶ -1;
		pak_sa DriveType::CONSTANT:
			ᬯᬮᬶ (puur_naa_nka)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			puur_naa_nka offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ᬬᬾᬦ᭄ (insertion.second) {
				ᬬᬾᬦ᭄ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ᬮᬶᬬ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ᬯᬮᬶ insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			puur_naa_nka offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ᬬᬾᬦ᭄ (insertion.second) {
				puur_naa_nka width = port_bit.cell->connections().at(port_bit.port).size();
				ᬬᬾᬦ᭄ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ᬮᬶᬬ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ᬯᬮᬶ insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ᬬᬾᬦ᭄ (found_isolated != isolated_drive_bits.end())
		ᬯᬮᬶ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ᬬᬾᬦ᭄ (found == drive_bits.begin()) {
		ᬯᬮᬶ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ᬬᬾᬦ᭄ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ᬮᬶᬬ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ᬯᬮᬶ result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ᬬᬾᬦ᭄ (driven_id == driver_id)
		ᬯᬮᬶ;

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

	ᬬᬾᬦ᭄ (a_id == b_id)
		ᬯᬮᬶ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ᬬᬾᬦ᭄ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ᬮᬶᬬ ᬬᬾᬦ᭄ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ᬮᬶᬬ ᬬᬾᬦ᭄ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ᬮᬶᬬ ᬬᬾᬦ᭄ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ᬮᬶᬬ
		connect_undirected(a_id, b_id);

	ᬯᬮᬶ;
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
	ᬲᭂᬤᭂᬮ (a_chunk != a_end) {
		puur_naa_nka a_width = a_chunk->width - a_offset;
		ᬬᬾᬦ᭄ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		puur_naa_nka b_width = b_chunk->width - b_offset;
		ᬬᬾᬦ᭄ (b_width == 0) {
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

		ᬬᬾᬦ᭄ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ᬬᬾᬦ᭄ (connected_drivers.contains(current))
			drivers.emplace(current);

		puur_naa_nka undirected_driver_count = connected_undirected.count(current);

		for (puur_naa_nka i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ᬬᬾᬦ᭄ (drivers.empty())
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
				ᬬᬾᬦ᭄ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	for (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ᬬᬾᬦ᭄ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ᬯᬮᬶ bit;
	ᬬᬾᬦ᭄ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		for (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ᬯᬮᬶ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ᬬᬾᬦ᭄ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ᬯᬮᬶ bit_repr;

	puur_naa_nka implicit_driver_count = connected_drivers.count(bit_repr_id);
	ᬬᬾᬦ᭄ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ᬬᬾᬦ᭄ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	for (puur_naa_nka i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	puur_naa_nka oriented_driver_count = connected_oriented.count(bit_repr_id);
	for (puur_naa_nka i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ᬯᬮᬶ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	for (puur_naa_nka i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ᬯᬮᬶ result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ᬬᬾᬦ᭄ (chunk.is_whole())
		ᬯᬮᬶ id;
	ᬬᬾᬦ᭄ (chunk.width == 1)
		ᬯᬮᬶ stringf("%s [%d]", id, chunk.offset);
	ᬯᬮᬶ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ᬬᬾᬦ᭄ (chunk.is_whole())
		ᬯᬮᬶ stringf("%s <%s>", cell_id, port_id);
	ᬬᬾᬦ᭄ (chunk.width == 1)
		ᬯᬮᬶ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ᬯᬮᬶ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ᬬᬾᬦ᭄ (chunk.width == 1)
		ᬯᬮᬶ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ᬯᬮᬶ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ᬯᬮᬶ stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ᬯᬮᬶ log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ᬯᬮᬶ log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ᬯᬮᬶ log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ᬯᬮᬶ log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			for (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ᬯᬮᬶ str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ᬬᬾᬦ᭄ (chunks.empty())
		ᬯᬮᬶ "{}";
	ᬬᬾᬦ᭄ (chunks.size() == 1)
		ᬯᬮᬶ log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	for (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ᬯᬮᬶ str;
}

YOSYS_NAMESPACE_END
