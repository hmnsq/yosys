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
	ប្រសិនបើ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	បើមិនដូច្នេះ
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	ប្រសិនបើ (other.type_ == DriveType::NONE)
		ត្រឡប់;
	ប្រសិនបើ (type_ == DriveType::NONE) {
		*this = other;
		ត្រឡប់;
	}
	ប្រសិនបើ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	ប្រសិនបើ (single.type() == DriveType::NONE)
		ត្រឡប់;
	ប្រសិនបើ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ត្រឡប់;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	ប្រសិនបើ (single.type() == DriveType::NONE)
		ត្រឡប់;
	ប្រសិនបើ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ត្រឡប់;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ចំនួនគត់ i) const
{
	DriveBitMultiple result;
	សម្រាប់ (auto const &single : multiple_)
		result.merge(single[i]);
	ត្រឡប់ result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ត្រឡប់ bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	ប្រសិនបើ (!can_append(bit))
		ត្រឡប់ false;
	width += 1;
	ត្រឡប់ true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	ប្រសិនបើ (chunk.wire != wire || chunk.offset != offset + width)
		ត្រឡប់ false;
	width += chunk.width;
	ត្រឡប់ true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ត្រឡប់ bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	ប្រសិនបើ (!can_append(bit))
		ត្រឡប់ false;
	width += 1;
	ត្រឡប់ true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	ប្រសិនបើ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ត្រឡប់ false;
	width += chunk.width;
	ត្រឡប់ true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ត្រឡប់ bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	ប្រសិនបើ (!can_append(bit))
		ត្រឡប់ false;
	width += 1;
	ត្រឡប់ true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	ប្រសិនបើ (chunk.marker != marker || chunk.offset != offset + width)
		ត្រឡប់ false;
	width += chunk.width;
	ត្រឡប់ true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	ប្រសិនបើ (bit.multiple().size() != multiple_.size())
		ត្រឡប់ false;

	ចំនួនគត់ const_drivers = 0;
	សម្រាប់ (DriveChunk const &single : multiple_)
		ប្រសិនបើ (single.is_constant())
			const_drivers += 1;

	ប្រសិនបើ (const_drivers > 1)
		ត្រឡប់ false;

	សម្រាប់ (DriveBit const &single : bit.multiple())
		ប្រសិនបើ (single.is_constant())
			const_drivers -= 1;

	ប្រសិនបើ (const_drivers != 0)
		ត្រឡប់ false;

	សម្រាប់ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				ប្រសិនបើ (!bit.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				ប្រសិនបើ (!bit.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				ប្រសិនបើ (!bit.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			default:
				ត្រឡប់ false;
		}
	}

	ត្រឡប់ true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	ប្រសិនបើ (chunk.multiple().size() != multiple_.size())
		ត្រឡប់ false;

	ចំនួនគត់ const_drivers = 0;
	សម្រាប់ (DriveChunk const &single : multiple_)
		ប្រសិនបើ (single.is_constant())
			const_drivers += 1;

	ប្រសិនបើ (const_drivers > 1)
		ត្រឡប់ false;

	សម្រាប់ (DriveChunk const &single : chunk.multiple())
		ប្រសិនបើ (single.is_constant())
			const_drivers -= 1;

	ប្រសិនបើ (const_drivers != 0)
		ត្រឡប់ false;

	សម្រាប់ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				ប្រសិនបើ (!chunk.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				ប្រសិនបើ (!chunk.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				ប្រសិនបើ (!chunk.multiple().count(next))
					ត្រឡប់ false;
			} bha_nga;
			default:
				ត្រឡប់ false;
		}
	}

	ត្រឡប់ true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	ប្រសិនបើ (!can_append(bit))
		ត្រឡប់ false;
	width_ += 1;
	State constant;

	សម្រាប់ (DriveBit const &single : bit.multiple())
		ប្រសិនបើ (single.is_constant())
			constant = single.constant();

	សម្រាប់ (DriveChunk &single : multiple_)
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
	ត្រឡប់ true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	ប្រសិនបើ (!can_append(chunk))
		ត្រឡប់ false;
	ចំនួនគត់ width = chunk.size();
	width_ += width;
	Const constant;

	សម្រាប់ (DriveChunk const &single : chunk.multiple())
		ប្រសិនបើ (single.is_constant())
			constant = single.constant();

	សម្រាប់ (DriveChunk &single : multiple_)
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
	ត្រឡប់ true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	ប្រសិនបើ (size() == 0)
		ត្រឡប់ true;
	ប្រសិនបើ (bit.type() != type_)
		ត្រឡប់ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ត្រឡប់ true;
		pak_sa DriveType::CONSTANT:
			ត្រឡប់ true;
		pak_sa DriveType::WIRE:
			ត្រឡប់ wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ត្រឡប់ port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ត្រឡប់ multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	ប្រសិនបើ (size() == 0)
		*this = bit;
	ប្រសិនបើ (bit.type() != type_)
		ត្រឡប់ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ត្រឡប់ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ត្រឡប់ true;
		pak_sa DriveType::WIRE:
			ត្រឡប់ wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ត្រឡប់ port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ត្រឡប់ multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	ប្រសិនបើ (size() == 0)
		*this = chunk;
	ប្រសិនបើ (chunk.type_ != type_)
		ត្រឡប់ false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ត្រឡប់ true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ត្រឡប់ true;
		pak_sa DriveType::WIRE:
			ត្រឡប់ wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ត្រឡប់ port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ត្រឡប់ marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ត្រឡប់ multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	ប្រសិនបើ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ត្រឡប់;
	}

	ប្រសិនបើ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	ប្រសិនបើ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	ប្រសិនបើ (bits_.empty())
		ត្រឡប់;
	std::vector<DriveBit> bits(std::move(bits_));
	សម្រាប់ (auto &bit : bits)
		ប្រសិនបើ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	ប្រសិនបើ (chunks_.empty())
		ត្រឡប់;
	សម្រាប់ (auto &chunk : chunks_)
	{
		សម្រាប់ (ចំនួនគត់ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	សម្រាប់ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	ប្រសិនបើ (first_edges.emplace(src, dst).first->second == dst)
		ត្រឡប់;
	ប្រសិនបើ (second_edges.emplace(src, dst).first->second == dst)
		ត្រឡប់;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	ប្រសិនបើ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		ប្រសិនបើ (found_more->second.empty())
			more_edges.erase(found_more);
		ត្រឡប់ result;
	}

	auto found_second = second_edges.find(src);
	ប្រសិនបើ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ត្រឡប់ result;
	}

	auto found_first = first_edges.find(src);
	ប្រសិនបើ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ត្រឡប់ result;
	}

	ត្រឡប់ DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ត្រឡប់ first_edges.count(src);
}

ចំនួនគត់ DriverMap::DriveBitGraph::count(DriveBitId src)
{
	ប្រសិនបើ (!first_edges.count(src))
		ត្រឡប់ 0;
	ប្រសិនបើ (!second_edges.count(src))
		ត្រឡប់ 1;
	auto found = more_edges.find(src);
	ប្រសិនបើ (found == more_edges.end())
		ត្រឡប់ 2;
	ត្រឡប់ GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ចំនួនគត់ index)
{
	ប្រសិនបើ (index == 0)
		ត្រឡប់ first_edges.at(src);
	បើមិនដូច្នេះ ប្រសិនបើ (index == 1)
		ត្រឡប់ second_edges.at(src);
	បើមិនដូច្នេះ
		ត្រឡប់ *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ត្រឡប់ BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ត្រឡប់ bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			ប្រសិនបើ (driver && !driven)
				ត្រឡប់ BitMode::DRIVER;
			បើមិនដូច្នេះ ប្រសិនបើ (driven && !driver)
				ត្រឡប់ BitMode::DRIVEN;
			បើមិនដូច្នេះ ប្រសិនបើ (driver && driven)
				ត្រឡប់ BitMode::TRISTATE;
			បើមិនដូច្នេះ
				ត្រឡប់ keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			ប្រសិនបើ (driver && !driven)
				ត្រឡប់ BitMode::DRIVER;
			បើមិនដូច្នេះ ប្រសិនបើ (driven && !driver)
				ត្រឡប់ BitMode::DRIVEN_UNIQUE;
			បើមិនដូច្នេះ
				ត្រឡប់ BitMode::TRISTATE;
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
			ត្រឡប់ -1;
		pak_sa DriveType::CONSTANT:
			ត្រឡប់ (ចំនួនគត់)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ចំនួនគត់ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			ប្រសិនបើ (insertion.second) {
				ប្រសិនបើ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} បើមិនដូច្នេះ
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ត្រឡប់ insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ចំនួនគត់ offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			ប្រសិនបើ (insertion.second) {
				ចំនួនគត់ width = port_bit.cell->connections().at(port_bit.port).size();
				ប្រសិនបើ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} បើមិនដូច្នេះ
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ត្រឡប់ insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	ប្រសិនបើ (found_isolated != isolated_drive_bits.end())
		ត្រឡប់ found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	ប្រសិនបើ (found == drive_bits.begin()) {
		ត្រឡប់ id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	ប្រសិនបើ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} បើមិនដូច្នេះ {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ត្រឡប់ result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	ប្រសិនបើ (driven_id == driver_id)
		ត្រឡប់;

	same_driver.merge(driven_id, driver_id);

	សម្រាប់ (ចំនួនគត់ i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	សម្រាប់ (ចំនួនគត់ i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	សម្រាប់ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	សម្រាប់ (auto cell : module->cells())
		សម្រាប់ (auto const &conn : cell->connections())
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

	ប្រសិនបើ (a_id == b_id)
		ត្រឡប់;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	ប្រសិនបើ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	បើមិនដូច្នេះ ប្រសិនបើ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	បើមិនដូច្នេះ ប្រសិនបើ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	បើមិនដូច្នេះ ប្រសិនបើ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	បើមិនដូច្នេះ ប្រសិនបើ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	បើមិនដូច្នេះ ប្រសិនបើ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	បើមិនដូច្នេះ
		connect_undirected(a_id, b_id);

	ត្រឡប់;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ចំនួនគត់ a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ចំនួនគត់ b_offset = 0;

	SigChunk tmp_a, tmp_b;
	ខណៈពេល (a_chunk != a_end) {
		ចំនួនគត់ a_width = a_chunk->width - a_offset;
		ប្រសិនបើ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ចំនួនគត់ b_width = b_chunk->width - b_offset;
		ប្រសិនបើ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		ចំនួនគត់ width = std::min(a_width, b_width);
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
	ចំនួនគត់ offset = 0;
	សម្រាប់ (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ចំនួនគត់> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	សម្រាប់ (ចំនួនគត់ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		ប្រសិនបើ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		ប្រសិនបើ (connected_drivers.contains(current))
			drivers.emplace(current);

		ចំនួនគត់ undirected_driver_count = connected_undirected.count(current);

		សម្រាប់ (ចំនួនគត់ i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	ប្រសិនបើ (drivers.empty())
		សម្រាប់ (auto seen_id : seen)
			drivers.emplace(seen_id);

	សម្រាប់ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		សម្រាប់ (ចំនួនគត់ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ចំនួនគត់ undirected_driver_count = connected_undirected.count(current);

			សម្រាប់ (ចំនួនគត់ i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				ប្រសិនបើ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	សម្រាប់ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	ប្រសិនបើ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ត្រឡប់ bit;
	ប្រសិនបើ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		សម្រាប់ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ត្រឡប់ result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	ប្រសិនបើ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ត្រឡប់ bit_repr;

	ចំនួនគត់ implicit_driver_count = connected_drivers.count(bit_repr_id);
	ប្រសិនបើ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	ប្រសិនបើ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	សម្រាប់ (ចំនួនគត់ i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ចំនួនគត់ oriented_driver_count = connected_oriented.count(bit_repr_id);
	សម្រាប់ (ចំនួនគត់ i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ត្រឡប់ driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	សម្រាប់ (ចំនួនគត់ i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ត្រឡប់ result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	ប្រសិនបើ (chunk.is_whole())
		ត្រឡប់ id;
	ប្រសិនបើ (chunk.width == 1)
		ត្រឡប់ stringf("%s [%d]", id, chunk.offset);
	ត្រឡប់ stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	ប្រសិនបើ (chunk.is_whole())
		ត្រឡប់ stringf("%s <%s>", cell_id, port_id);
	ប្រសិនបើ (chunk.width == 1)
		ត្រឡប់ stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ត្រឡប់ stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	ប្រសិនបើ (chunk.width == 1)
		ត្រឡប់ stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ត្រឡប់ stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ត្រឡប់ stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ត្រឡប់ log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ត្រឡប់ log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ត្រឡប់ log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ត្រឡប់ log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			សម្រាប់ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ត្រឡប់ str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	ប្រសិនបើ (chunks.empty())
		ត្រឡប់ "{}";
	ប្រសិនបើ (chunks.size() == 1)
		ត្រឡប់ log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	សម្រាប់ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ត្រឡប់ str;
}

YOSYS_NAMESPACE_END
