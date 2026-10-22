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
	གལ་སྲིད (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	དེ་མིན
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	གལ་སྲིད (other.type_ == DriveType::NONE)
		ལོག;
	གལ་སྲིད (type_ == DriveType::NONE) {
		*this = other;
		ལོག;
	}
	གལ་སྲིད (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	གལ་སྲིད (single.type() == DriveType::NONE)
		ལོག;
	གལ་སྲིད (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ལོག;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	གལ་སྲིད (single.type() == DriveType::NONE)
		ལོག;
	གལ་སྲིད (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ལོག;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ཧྲིལ i) const
{
	DriveBitMultiple result;
	དོན་ལུ (auto const &single : multiple_)
		result.merge(single[i]);
	ལོག result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ལོག bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	གལ་སྲིད (!can_append(bit))
		ལོག false;
	width += 1;
	ལོག true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	གལ་སྲིད (chunk.wire != wire || chunk.offset != offset + width)
		ལོག false;
	width += chunk.width;
	ལོག true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ལོག bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	གལ་སྲིད (!can_append(bit))
		ལོག false;
	width += 1;
	ལོག true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	གལ་སྲིད (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ལོག false;
	width += chunk.width;
	ལོག true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ལོག bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	གལ་སྲིད (!can_append(bit))
		ལོག false;
	width += 1;
	ལོག true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	གལ་སྲིད (chunk.marker != marker || chunk.offset != offset + width)
		ལོག false;
	width += chunk.width;
	ལོག true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	གལ་སྲིད (bit.multiple().size() != multiple_.size())
		ལོག false;

	ཧྲིལ const_drivers = 0;
	དོན་ལུ (DriveChunk const &single : multiple_)
		གལ་སྲིད (single.is_constant())
			const_drivers += 1;

	གལ་སྲིད (const_drivers > 1)
		ལོག false;

	དོན་ལུ (DriveBit const &single : bit.multiple())
		གལ་སྲིད (single.is_constant())
			const_drivers -= 1;

	གལ་སྲིད (const_drivers != 0)
		ལོག false;

	དོན་ལུ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				གལ་སྲིད (!bit.multiple().count(next))
					ལོག false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				གལ་སྲིད (!bit.multiple().count(next))
					ལོག false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				གལ་སྲིད (!bit.multiple().count(next))
					ལོག false;
			} bha_nga;
			default:
				ལོག false;
		}
	}

	ལོག true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	གལ་སྲིད (chunk.multiple().size() != multiple_.size())
		ལོག false;

	ཧྲིལ const_drivers = 0;
	དོན་ལུ (DriveChunk const &single : multiple_)
		གལ་སྲིད (single.is_constant())
			const_drivers += 1;

	གལ་སྲིད (const_drivers > 1)
		ལོག false;

	དོན་ལུ (DriveChunk const &single : chunk.multiple())
		གལ་སྲིད (single.is_constant())
			const_drivers -= 1;

	གལ་སྲིད (const_drivers != 0)
		ལོག false;

	དོན་ལུ (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				གལ་སྲིད (!chunk.multiple().count(next))
					ལོག false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				གལ་སྲིད (!chunk.multiple().count(next))
					ལོག false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				གལ་སྲིད (!chunk.multiple().count(next))
					ལོག false;
			} bha_nga;
			default:
				ལོག false;
		}
	}

	ལོག true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	གལ་སྲིད (!can_append(bit))
		ལོག false;
	width_ += 1;
	State constant;

	དོན་ལུ (DriveBit const &single : bit.multiple())
		གལ་སྲིད (single.is_constant())
			constant = single.constant();

	དོན་ལུ (DriveChunk &single : multiple_)
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
	ལོག true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	གལ་སྲིད (!can_append(chunk))
		ལོག false;
	ཧྲིལ width = chunk.size();
	width_ += width;
	Const constant;

	དོན་ལུ (DriveChunk const &single : chunk.multiple())
		གལ་སྲིད (single.is_constant())
			constant = single.constant();

	དོན་ལུ (DriveChunk &single : multiple_)
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
	ལོག true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	གལ་སྲིད (size() == 0)
		ལོག true;
	གལ་སྲིད (bit.type() != type_)
		ལོག false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ལོག true;
		pak_sa DriveType::CONSTANT:
			ལོག true;
		pak_sa DriveType::WIRE:
			ལོག wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ལོག port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ལོག multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	གལ་སྲིད (size() == 0)
		*this = bit;
	གལ་སྲིད (bit.type() != type_)
		ལོག false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ལོག true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ལོག true;
		pak_sa DriveType::WIRE:
			ལོག wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ལོག port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ལོག multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	གལ་སྲིད (size() == 0)
		*this = chunk;
	གལ་སྲིད (chunk.type_ != type_)
		ལོག false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ལོག true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ལོག true;
		pak_sa DriveType::WIRE:
			ལོག wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ལོག port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ལོག marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ལོག multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	གལ་སྲིད (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ལོག;
	}

	གལ་སྲིད (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	གལ་སྲིད (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	གལ་སྲིད (bits_.empty())
		ལོག;
	std::vector<DriveBit> bits(std::move(bits_));
	དོན་ལུ (auto &bit : bits)
		གལ་སྲིད (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	གལ་སྲིད (chunks_.empty())
		ལོག;
	དོན་ལུ (auto &chunk : chunks_)
	{
		དོན་ལུ (ཧྲིལ i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	དོན་ལུ (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	གལ་སྲིད (first_edges.emplace(src, dst).first->second == dst)
		ལོག;
	གལ་སྲིད (second_edges.emplace(src, dst).first->second == dst)
		ལོག;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	གལ་སྲིད (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		གལ་སྲིད (found_more->second.empty())
			more_edges.erase(found_more);
		ལོག result;
	}

	auto found_second = second_edges.find(src);
	གལ་སྲིད (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ལོག result;
	}

	auto found_first = first_edges.find(src);
	གལ་སྲིད (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ལོག result;
	}

	ལོག DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ལོག first_edges.count(src);
}

ཧྲིལ DriverMap::DriveBitGraph::count(DriveBitId src)
{
	གལ་སྲིད (!first_edges.count(src))
		ལོག 0;
	གལ་སྲིད (!second_edges.count(src))
		ལོག 1;
	auto found = more_edges.find(src);
	གལ་སྲིད (found == more_edges.end())
		ལོག 2;
	ལོག GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ཧྲིལ index)
{
	གལ་སྲིད (index == 0)
		ལོག first_edges.at(src);
	དེ་མིན གལ་སྲིད (index == 1)
		ལོག second_edges.at(src);
	དེ་མིན
		ལོག *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ལོག BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ལོག bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			གལ་སྲིད (driver && !driven)
				ལོག BitMode::DRIVER;
			དེ་མིན གལ་སྲིད (driven && !driver)
				ལོག BitMode::DRIVEN;
			དེ་མིན གལ་སྲིད (driver && driven)
				ལོག BitMode::TRISTATE;
			དེ་མིན
				ལོག keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			གལ་སྲིད (driver && !driven)
				ལོག BitMode::DRIVER;
			དེ་མིན གལ་སྲིད (driven && !driver)
				ལོག BitMode::DRIVEN_UNIQUE;
			དེ་མིན
				ལོག BitMode::TRISTATE;
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
			ལོག -1;
		pak_sa DriveType::CONSTANT:
			ལོག (ཧྲིལ)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ཧྲིལ offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			གལ་སྲིད (insertion.second) {
				གལ་སྲིད (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} དེ་མིན
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ལོག insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ཧྲིལ offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			གལ་སྲིད (insertion.second) {
				ཧྲིལ width = port_bit.cell->connections().at(port_bit.port).size();
				གལ་སྲིད (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} དེ་མིན
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ལོག insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	གལ་སྲིད (found_isolated != isolated_drive_bits.end())
		ལོག found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	གལ་སྲིད (found == drive_bits.begin()) {
		ལོག id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	གལ་སྲིད (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} དེ་མིན {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ལོག result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	གལ་སྲིད (driven_id == driver_id)
		ལོག;

	same_driver.merge(driven_id, driver_id);

	དོན་ལུ (ཧྲིལ i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	དོན་ལུ (ཧྲིལ i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	དོན་ལུ (auto const &conn : module->connections())
		add(conn.first, conn.second);

	དོན་ལུ (auto cell : module->cells())
		དོན་ལུ (auto const &conn : cell->connections())
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

	གལ་སྲིད (a_id == b_id)
		ལོག;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	གལ་སྲིད (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	དེ་མིན གལ་སྲིད (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	དེ་མིན གལ་སྲིད (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	དེ་མིན གལ་སྲིད (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	དེ་མིན གལ་སྲིད (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	དེ་མིན གལ་སྲིད (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	དེ་མིན
		connect_undirected(a_id, b_id);

	ལོག;
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
		གལ་སྲིད (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ཧྲིལ b_width = b_chunk->width - b_offset;
		གལ་སྲིད (b_width == 0) {
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
	དོན་ལུ (auto const &chunk : b.chunks()) {
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

	དོན་ལུ (ཧྲིལ pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		གལ་སྲིད (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		གལ་སྲིད (connected_drivers.contains(current))
			drivers.emplace(current);

		ཧྲིལ undirected_driver_count = connected_undirected.count(current);

		དོན་ལུ (ཧྲིལ i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	གལ་སྲིད (drivers.empty())
		དོན་ལུ (auto seen_id : seen)
			drivers.emplace(seen_id);

	དོན་ལུ (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		དོན་ལུ (ཧྲིལ pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ཧྲིལ undirected_driver_count = connected_undirected.count(current);

			དོན་ལུ (ཧྲིལ i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				གལ་སྲིད (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	དོན་ལུ (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	གལ་སྲིད (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ལོག bit;
	གལ་སྲིད (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		དོན་ལུ (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ལོག result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	གལ་སྲིད (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ལོག bit_repr;

	ཧྲིལ implicit_driver_count = connected_drivers.count(bit_repr_id);
	གལ་སྲིད (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	གལ་སྲིད (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	དོན་ལུ (ཧྲིལ i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ཧྲིལ oriented_driver_count = connected_oriented.count(bit_repr_id);
	དོན་ལུ (ཧྲིལ i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ལོག driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	དོན་ལུ (ཧྲིལ i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ལོག result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	གལ་སྲིད (chunk.is_whole())
		ལོག id;
	གལ་སྲིད (chunk.width == 1)
		ལོག stringf("%s [%d]", id, chunk.offset);
	ལོག stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	གལ་སྲིད (chunk.is_whole())
		ལོག stringf("%s <%s>", cell_id, port_id);
	གལ་སྲིད (chunk.width == 1)
		ལོག stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ལོག stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	གལ་སྲིད (chunk.width == 1)
		ལོག stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ལོག stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ལོག stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ལོག log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ལོག log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ལོག log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ལོག log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			དོན་ལུ (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ལོག str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	གལ་སྲིད (chunks.empty())
		ལོག "{}";
	གལ་སྲིད (chunks.size() == 1)
		ལོག log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	དོན་ལུ (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ལོག str;
}

YOSYS_NAMESPACE_END
