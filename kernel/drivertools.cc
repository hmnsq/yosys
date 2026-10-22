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
	ຖ້າ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	ບໍ່ດັ່ງນັ້ນ
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ຖ້າ (other.type_ == DriveType::NONE)
		ສົ່ງຄືນ;
	ຖ້າ (type_ == DriveType::NONE) {
		*this = other;
		ສົ່ງຄືນ;
	}
	ຖ້າ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ຖ້າ (single.type() == DriveType::NONE)
		ສົ່ງຄືນ;
	ຖ້າ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ສົ່ງຄືນ;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ຖ້າ (single.type() == DriveType::NONE)
		ສົ່ງຄືນ;
	ຖ້າ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ສົ່ງຄືນ;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ຈຳນວນເຕັມ i) const
{
	DriveBitMultiple result;
	ສຳລັບ (auto const &single : multiple_)
		result.merge(single[i]);
	ສົ່ງຄືນ result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ສົ່ງຄືນ bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ຖ້າ (!can_append(bit))
		ສົ່ງຄືນ false;
	width += 1;
	ສົ່ງຄືນ true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ຖ້າ (chunk.wire != wire || chunk.offset != offset + width)
		ສົ່ງຄືນ false;
	width += chunk.width;
	ສົ່ງຄືນ true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ສົ່ງຄືນ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ຖ້າ (!can_append(bit))
		ສົ່ງຄືນ false;
	width += 1;
	ສົ່ງຄືນ true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ຖ້າ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ສົ່ງຄືນ false;
	width += chunk.width;
	ສົ່ງຄືນ true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ສົ່ງຄືນ bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ຖ້າ (!can_append(bit))
		ສົ່ງຄືນ false;
	width += 1;
	ສົ່ງຄືນ true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ຖ້າ (chunk.marker != marker || chunk.offset != offset + width)
		ສົ່ງຄືນ false;
	width += chunk.width;
	ສົ່ງຄືນ true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ຖ້າ (bit.multiple().size() != multiple_.size())
		ສົ່ງຄືນ false;

	ຈຳນວນເຕັມ const_drivers = 0;
	ສຳລັບ (DriveChunk const &single : multiple_)
		ຖ້າ (single.is_constant())
			const_drivers += 1;

	ຖ້າ (const_drivers > 1)
		ສົ່ງຄືນ false;

	ສຳລັບ (DriveBit const &single : bit.multiple())
		ຖ້າ (single.is_constant())
			const_drivers -= 1;

	ຖ້າ (const_drivers != 0)
		ສົ່ງຄືນ false;

	ສຳລັບ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ຖ້າ (!bit.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ຖ້າ (!bit.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ຖ້າ (!bit.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			default:
				ສົ່ງຄືນ false;
		}
	}

	ສົ່ງຄືນ true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ຖ້າ (chunk.multiple().size() != multiple_.size())
		ສົ່ງຄືນ false;

	ຈຳນວນເຕັມ const_drivers = 0;
	ສຳລັບ (DriveChunk const &single : multiple_)
		ຖ້າ (single.is_constant())
			const_drivers += 1;

	ຖ້າ (const_drivers > 1)
		ສົ່ງຄືນ false;

	ສຳລັບ (DriveChunk const &single : chunk.multiple())
		ຖ້າ (single.is_constant())
			const_drivers -= 1;

	ຖ້າ (const_drivers != 0)
		ສົ່ງຄືນ false;

	ສຳລັບ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ຖ້າ (!chunk.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ຖ້າ (!chunk.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ຖ້າ (!chunk.multiple().count(next))
					ສົ່ງຄືນ false;
			} bha_nga;
			default:
				ສົ່ງຄືນ false;
		}
	}

	ສົ່ງຄືນ true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ຖ້າ (!can_append(bit))
		ສົ່ງຄືນ false;
	width_ += 1;
	State constant;

	ສຳລັບ (DriveBit const &single : bit.multiple())
		ຖ້າ (single.is_constant())
			constant = single.constant();

	ສຳລັບ (DriveChunk &single : multiple_)
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
	ສົ່ງຄືນ true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ຖ້າ (!can_append(chunk))
		ສົ່ງຄືນ false;
	ຈຳນວນເຕັມ width = chunk.size();
	width_ += width;
	Const constant;

	ສຳລັບ (DriveChunk const &single : chunk.multiple())
		ຖ້າ (single.is_constant())
			constant = single.constant();

	ສຳລັບ (DriveChunk &single : multiple_)
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
	ສົ່ງຄືນ true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ຖ້າ (size() == 0)
		ສົ່ງຄືນ true;
	ຖ້າ (bit.type() != type_)
		ສົ່ງຄືນ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ສົ່ງຄືນ true;
		pak_sa DriveType::CONSTANT:
			ສົ່ງຄືນ true;
		pak_sa DriveType::WIRE:
			ສົ່ງຄືນ wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ສົ່ງຄືນ port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ສົ່ງຄືນ multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ຖ້າ (size() == 0)
		*this = bit;
	ຖ້າ (bit.type() != type_)
		ສົ່ງຄືນ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ສົ່ງຄືນ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ສົ່ງຄືນ true;
		pak_sa DriveType::WIRE:
			ສົ່ງຄືນ wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ສົ່ງຄືນ port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ສົ່ງຄືນ multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ຖ້າ (size() == 0)
		*this = chunk;
	ຖ້າ (chunk.type_ != type_)
		ສົ່ງຄືນ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ສົ່ງຄືນ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ສົ່ງຄືນ true;
		pak_sa DriveType::WIRE:
			ສົ່ງຄືນ wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ສົ່ງຄືນ port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ສົ່ງຄືນ marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ສົ່ງຄືນ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ຖ້າ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ສົ່ງຄືນ;
	}

	ຖ້າ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ຖ້າ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ຖ້າ (bits_.empty())
		ສົ່ງຄືນ;
	std::vector<DriveBit> bits(std::move(bits_));
	ສຳລັບ (auto &bit : bits)
		ຖ້າ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ຖ້າ (chunks_.empty())
		ສົ່ງຄືນ;
	ສຳລັບ (auto &chunk : chunks_)
	{
		ສຳລັບ (ຈຳນວນເຕັມ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	ສຳລັບ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ຖ້າ (first_edges.emplace(src, dst).first->second == dst)
		ສົ່ງຄືນ;
	ຖ້າ (second_edges.emplace(src, dst).first->second == dst)
		ສົ່ງຄືນ;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ຖ້າ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ຖ້າ (found_more->second.empty())
			more_edges.erase(found_more);
		ສົ່ງຄືນ result;
	}

	auto found_second = second_edges.find(src);
	ຖ້າ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ສົ່ງຄືນ result;
	}

	auto found_first = first_edges.find(src);
	ຖ້າ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ສົ່ງຄືນ result;
	}

	ສົ່ງຄືນ DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ສົ່ງຄືນ first_edges.count(src);
}

ຈຳນວນເຕັມ DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ຖ້າ (!first_edges.count(src))
		ສົ່ງຄືນ 0;
	ຖ້າ (!second_edges.count(src))
		ສົ່ງຄືນ 1;
	auto found = more_edges.find(src);
	ຖ້າ (found == more_edges.end())
		ສົ່ງຄືນ 2;
	ສົ່ງຄືນ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ຈຳນວນເຕັມ index)
{
	ຖ້າ (index == 0)
		ສົ່ງຄືນ first_edges.at(src);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (index == 1)
		ສົ່ງຄືນ second_edges.at(src);
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ສົ່ງຄືນ BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ສົ່ງຄືນ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ຖ້າ (driver && !driven)
				ສົ່ງຄືນ BitMode::DRIVER;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (driven && !driver)
				ສົ່ງຄືນ BitMode::DRIVEN;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (driver && driven)
				ສົ່ງຄືນ BitMode::TRISTATE;
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ຖ້າ (driver && !driven)
				ສົ່ງຄືນ BitMode::DRIVER;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (driven && !driver)
				ສົ່ງຄືນ BitMode::DRIVEN_UNIQUE;
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ BitMode::TRISTATE;
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
			ສົ່ງຄືນ -1;
		pak_sa DriveType::CONSTANT:
			ສົ່ງຄືນ (ຈຳນວນເຕັມ)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ຈຳນວນເຕັມ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ຖ້າ (insertion.second) {
				ຖ້າ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ບໍ່ດັ່ງນັ້ນ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ສົ່ງຄືນ insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ຈຳນວນເຕັມ offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ຖ້າ (insertion.second) {
				ຈຳນວນເຕັມ width = port_bit.cell->connections().at(port_bit.port).size();
				ຖ້າ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} ບໍ່ດັ່ງນັ້ນ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ສົ່ງຄືນ insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ຖ້າ (found_isolated != isolated_drive_bits.end())
		ສົ່ງຄືນ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ຖ້າ (found == drive_bits.begin()) {
		ສົ່ງຄືນ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ຖ້າ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} ບໍ່ດັ່ງນັ້ນ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ສົ່ງຄືນ result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ຖ້າ (driven_id == driver_id)
		ສົ່ງຄືນ;

	same_driver.merge(driven_id, driver_id);

	ສຳລັບ (ຈຳນວນເຕັມ i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ສຳລັບ (ຈຳນວນເຕັມ i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	ສຳລັບ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	ສຳລັບ (auto cell : module->cells())
		ສຳລັບ (auto const &conn : cell->connections())
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

	ຖ້າ (a_id == b_id)
		ສົ່ງຄືນ;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ຖ້າ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	ບໍ່ດັ່ງນັ້ນ
		connect_undirected(a_id, b_id);

	ສົ່ງຄືນ;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ຈຳນວນເຕັມ a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ຈຳນວນເຕັມ b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ໃນຂະນະທີ່ (a_chunk != a_end) {
		ຈຳນວນເຕັມ a_width = a_chunk->width - a_offset;
		ຖ້າ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ຈຳນວນເຕັມ b_width = b_chunk->width - b_offset;
		ຖ້າ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		ຈຳນວນເຕັມ width = std::min(a_width, b_width);
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
	ຈຳນວນເຕັມ offset = 0;
	ສຳລັບ (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ຈຳນວນເຕັມ> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	ສຳລັບ (ຈຳນວນເຕັມ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ຖ້າ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ຖ້າ (connected_drivers.contains(current))
			drivers.emplace(current);

		ຈຳນວນເຕັມ undirected_driver_count = connected_undirected.count(current);

		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ຖ້າ (drivers.empty())
		ສຳລັບ (auto seen_id : seen)
			drivers.emplace(seen_id);

	ສຳລັບ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		ສຳລັບ (ຈຳນວນເຕັມ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ຈຳນວນເຕັມ undirected_driver_count = connected_undirected.count(current);

			ສຳລັບ (ຈຳນວນເຕັມ i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ຖ້າ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	ສຳລັບ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ຖ້າ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ສົ່ງຄືນ bit;
	ຖ້າ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		ສຳລັບ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ສົ່ງຄືນ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ຖ້າ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ສົ່ງຄືນ bit_repr;

	ຈຳນວນເຕັມ implicit_driver_count = connected_drivers.count(bit_repr_id);
	ຖ້າ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ຖ້າ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ຈຳນວນເຕັມ oriented_driver_count = connected_oriented.count(bit_repr_id);
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ສົ່ງຄືນ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ສົ່ງຄືນ result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ຖ້າ (chunk.is_whole())
		ສົ່ງຄືນ id;
	ຖ້າ (chunk.width == 1)
		ສົ່ງຄືນ stringf("%s [%d]", id, chunk.offset);
	ສົ່ງຄືນ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ຖ້າ (chunk.is_whole())
		ສົ່ງຄືນ stringf("%s <%s>", cell_id, port_id);
	ຖ້າ (chunk.width == 1)
		ສົ່ງຄືນ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ສົ່ງຄືນ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ຖ້າ (chunk.width == 1)
		ສົ່ງຄືນ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ສົ່ງຄືນ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ສົ່ງຄືນ stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ສົ່ງຄືນ log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ສົ່ງຄືນ log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ສົ່ງຄືນ log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ສົ່ງຄືນ log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			ສຳລັບ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ສົ່ງຄືນ str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ຖ້າ (chunks.empty())
		ສົ່ງຄືນ "{}";
	ຖ້າ (chunks.size() == 1)
		ສົ່ງຄືນ log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	ສຳລັບ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ສົ່ງຄືນ str;
}

YOSYS_NAMESPACE_END
