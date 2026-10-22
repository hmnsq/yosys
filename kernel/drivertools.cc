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
	གལ་ཏེ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	དེ་མིན
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	གལ་ཏེ (other.type_ == DriveType::NONE)
		ཕྱིར;
	གལ་ཏེ (type_ == DriveType::NONE) {
		*this = other;
		ཕྱིར;
	}
	གལ་ཏེ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	གལ་ཏེ (single.type() == DriveType::NONE)
		ཕྱིར;
	གལ་ཏེ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ཕྱིར;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	གལ་ཏེ (single.type() == DriveType::NONE)
		ཕྱིར;
	གལ་ཏེ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ཕྱིར;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ཧྲིལ i) const
{
	DriveBitMultiple result;
	ཆེད་དུ (auto const &single : multiple_)
		result.merge(single[i]);
	ཕྱིར result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ཕྱིར bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	གལ་ཏེ (!can_append(bit))
		ཕྱིར false;
	width += 1;
	ཕྱིར true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	གལ་ཏེ (chunk.wire != wire || chunk.offset != offset + width)
		ཕྱིར false;
	width += chunk.width;
	ཕྱིར true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ཕྱིར bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	གལ་ཏེ (!can_append(bit))
		ཕྱིར false;
	width += 1;
	ཕྱིར true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	གལ་ཏེ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ཕྱིར false;
	width += chunk.width;
	ཕྱིར true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ཕྱིར bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	གལ་ཏེ (!can_append(bit))
		ཕྱིར false;
	width += 1;
	ཕྱིར true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	གལ་ཏེ (chunk.marker != marker || chunk.offset != offset + width)
		ཕྱིར false;
	width += chunk.width;
	ཕྱིར true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	གལ་ཏེ (bit.multiple().size() != multiple_.size())
		ཕྱིར false;

	ཧྲིལ const_drivers = 0;
	ཆེད་དུ (DriveChunk const &single : multiple_)
		གལ་ཏེ (single.is_constant())
			const_drivers += 1;

	གལ་ཏེ (const_drivers > 1)
		ཕྱིར false;

	ཆེད་དུ (DriveBit const &single : bit.multiple())
		གལ་ཏེ (single.is_constant())
			const_drivers -= 1;

	གལ་ཏེ (const_drivers != 0)
		ཕྱིར false;

	ཆེད་དུ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				གལ་ཏེ (!bit.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				གལ་ཏེ (!bit.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				གལ་ཏེ (!bit.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			default:
				ཕྱིར false;
		}
	}

	ཕྱིར true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	གལ་ཏེ (chunk.multiple().size() != multiple_.size())
		ཕྱིར false;

	ཧྲིལ const_drivers = 0;
	ཆེད་དུ (DriveChunk const &single : multiple_)
		གལ་ཏེ (single.is_constant())
			const_drivers += 1;

	གལ་ཏེ (const_drivers > 1)
		ཕྱིར false;

	ཆེད་དུ (DriveChunk const &single : chunk.multiple())
		གལ་ཏེ (single.is_constant())
			const_drivers -= 1;

	གལ་ཏེ (const_drivers != 0)
		ཕྱིར false;

	ཆེད་དུ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				གལ་ཏེ (!chunk.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				གལ་ཏེ (!chunk.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				གལ་ཏེ (!chunk.multiple().count(next))
					ཕྱིར false;
			} bha_nga;
			default:
				ཕྱིར false;
		}
	}

	ཕྱིར true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	གལ་ཏེ (!can_append(bit))
		ཕྱིར false;
	width_ += 1;
	State constant;

	ཆེད་དུ (DriveBit const &single : bit.multiple())
		གལ་ཏེ (single.is_constant())
			constant = single.constant();

	ཆེད་དུ (DriveChunk &single : multiple_)
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
	ཕྱིར true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	གལ་ཏེ (!can_append(chunk))
		ཕྱིར false;
	ཧྲིལ width = chunk.size();
	width_ += width;
	Const constant;

	ཆེད་དུ (DriveChunk const &single : chunk.multiple())
		གལ་ཏེ (single.is_constant())
			constant = single.constant();

	ཆེད་དུ (DriveChunk &single : multiple_)
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
	ཕྱིར true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	གལ་ཏེ (size() == 0)
		ཕྱིར true;
	གལ་ཏེ (bit.type() != type_)
		ཕྱིར false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ཕྱིར true;
		pak_sa DriveType::CONSTANT:
			ཕྱིར true;
		pak_sa DriveType::WIRE:
			ཕྱིར wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ཕྱིར port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ཕྱིར multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	གལ་ཏེ (size() == 0)
		*this = bit;
	གལ་ཏེ (bit.type() != type_)
		ཕྱིར false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ཕྱིར true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ཕྱིར true;
		pak_sa DriveType::WIRE:
			ཕྱིར wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ཕྱིར port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ཕྱིར multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	གལ་ཏེ (size() == 0)
		*this = chunk;
	གལ་ཏེ (chunk.type_ != type_)
		ཕྱིར false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ཕྱིར true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ཕྱིར true;
		pak_sa DriveType::WIRE:
			ཕྱིར wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ཕྱིར port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ཕྱིར marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ཕྱིར multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	གལ་ཏེ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ཕྱིར;
	}

	གལ་ཏེ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	གལ་ཏེ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	གལ་ཏེ (bits_.empty())
		ཕྱིར;
	std::vector<DriveBit> bits(std::move(bits_));
	ཆེད་དུ (auto &bit : bits)
		གལ་ཏེ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	གལ་ཏེ (chunks_.empty())
		ཕྱིར;
	ཆེད་དུ (auto &chunk : chunks_)
	{
		ཆེད་དུ (ཧྲིལ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	ཆེད་དུ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	གལ་ཏེ (first_edges.emplace(src, dst).first->second == dst)
		ཕྱིར;
	གལ་ཏེ (second_edges.emplace(src, dst).first->second == dst)
		ཕྱིར;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	གལ་ཏེ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		གལ་ཏེ (found_more->second.empty())
			more_edges.erase(found_more);
		ཕྱིར result;
	}

	auto found_second = second_edges.find(src);
	གལ་ཏེ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ཕྱིར result;
	}

	auto found_first = first_edges.find(src);
	གལ་ཏེ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ཕྱིར result;
	}

	ཕྱིར DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ཕྱིར first_edges.count(src);
}

ཧྲིལ DriverMap::DriveBitGraph::count(DriveBitId src)
{
	གལ་ཏེ (!first_edges.count(src))
		ཕྱིར 0;
	གལ་ཏེ (!second_edges.count(src))
		ཕྱིར 1;
	auto found = more_edges.find(src);
	གལ་ཏེ (found == more_edges.end())
		ཕྱིར 2;
	ཕྱིར GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ཧྲིལ index)
{
	གལ་ཏེ (index == 0)
		ཕྱིར first_edges.at(src);
	དེ་མིན གལ་ཏེ (index == 1)
		ཕྱིར second_edges.at(src);
	དེ་མིན
		ཕྱིར *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ཕྱིར BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ཕྱིར bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			གལ་ཏེ (driver && !driven)
				ཕྱིར BitMode::DRIVER;
			དེ་མིན གལ་ཏེ (driven && !driver)
				ཕྱིར BitMode::DRIVEN;
			དེ་མིན གལ་ཏེ (driver && driven)
				ཕྱིར BitMode::TRISTATE;
			དེ་མིན
				ཕྱིར keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			གལ་ཏེ (driver && !driven)
				ཕྱིར BitMode::DRIVER;
			དེ་མིན གལ་ཏེ (driven && !driver)
				ཕྱིར BitMode::DRIVEN_UNIQUE;
			དེ་མིན
				ཕྱིར BitMode::TRISTATE;
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
			ཕྱིར -1;
		pak_sa DriveType::CONSTANT:
			ཕྱིར (ཧྲིལ)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ཧྲིལ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			གལ་ཏེ (insertion.second) {
				གལ་ཏེ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} དེ་མིན
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ཕྱིར insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ཧྲིལ offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			གལ་ཏེ (insertion.second) {
				ཧྲིལ width = port_bit.cell->connections().at(port_bit.port).size();
				གལ་ཏེ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} དེ་མིན
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ཕྱིར insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	གལ་ཏེ (found_isolated != isolated_drive_bits.end())
		ཕྱིར found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	གལ་ཏེ (found == drive_bits.begin()) {
		ཕྱིར id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	གལ་ཏེ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} དེ་མིན {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ཕྱིར result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	གལ་ཏེ (driven_id == driver_id)
		ཕྱིར;

	same_driver.merge(driven_id, driver_id);

	ཆེད་དུ (ཧྲིལ i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	ཆེད་དུ (ཧྲིལ i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	ཆེད་དུ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	ཆེད་དུ (auto cell : module->cells())
		ཆེད་དུ (auto const &conn : cell->connections())
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

	གལ་ཏེ (a_id == b_id)
		ཕྱིར;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	གལ་ཏེ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	དེ་མིན གལ་ཏེ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	དེ་མིན གལ་ཏེ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	དེ་མིན གལ་ཏེ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	དེ་མིན གལ་ཏེ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	དེ་མིན གལ་ཏེ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	དེ་མིན
		connect_undirected(a_id, b_id);

	ཕྱིར;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ཧྲིལ a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ཧྲིལ b_offset = 0;

	SigChunk tmp_a, tmp_b;
	སྐབས (a_chunk != a_end) {
		ཧྲིལ a_width = a_chunk->width - a_offset;
		གལ་ཏེ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ཧྲིལ b_width = b_chunk->width - b_offset;
		གལ་ཏེ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		ཧྲིལ width = std::min(a_width, b_width);
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
	ཧྲིལ offset = 0;
	ཆེད་དུ (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ཧྲིལ> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	ཆེད་དུ (ཧྲིལ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		གལ་ཏེ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		གལ་ཏེ (connected_drivers.contains(current))
			drivers.emplace(current);

		ཧྲིལ undirected_driver_count = connected_undirected.count(current);

		ཆེད་དུ (ཧྲིལ i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	གལ་ཏེ (drivers.empty())
		ཆེད་དུ (auto seen_id : seen)
			drivers.emplace(seen_id);

	ཆེད་དུ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		ཆེད་དུ (ཧྲིལ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ཧྲིལ undirected_driver_count = connected_undirected.count(current);

			ཆེད་དུ (ཧྲིལ i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				གལ་ཏེ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	ཆེད་དུ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	གལ་ཏེ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ཕྱིར bit;
	གལ་ཏེ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		ཆེད་དུ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ཕྱིར result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	གལ་ཏེ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ཕྱིར bit_repr;

	ཧྲིལ implicit_driver_count = connected_drivers.count(bit_repr_id);
	གལ་ཏེ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	གལ་ཏེ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	ཆེད་དུ (ཧྲིལ i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ཧྲིལ oriented_driver_count = connected_oriented.count(bit_repr_id);
	ཆེད་དུ (ཧྲིལ i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ཕྱིར driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	ཆེད་དུ (ཧྲིལ i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ཕྱིར result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	གལ་ཏེ (chunk.is_whole())
		ཕྱིར id;
	གལ་ཏེ (chunk.width == 1)
		ཕྱིར stringf("%s [%d]", id, chunk.offset);
	ཕྱིར stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	གལ་ཏེ (chunk.is_whole())
		ཕྱིར stringf("%s <%s>", cell_id, port_id);
	གལ་ཏེ (chunk.width == 1)
		ཕྱིར stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ཕྱིར stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	གལ་ཏེ (chunk.width == 1)
		ཕྱིར stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ཕྱིར stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ཕྱིར stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ཕྱིར log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ཕྱིར log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ཕྱིར log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ཕྱིར log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			ཆེད་དུ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ཕྱིར str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	གལ་ཏེ (chunks.empty())
		ཕྱིར "{}";
	གལ་ཏེ (chunks.size() == 1)
		ཕྱིར log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	ཆེད་དུ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ཕྱིར str;
}

YOSYS_NAMESPACE_END
