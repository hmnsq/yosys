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
	အကယ်၍ (bit.is_wire())
		*this = DriveBitWire(bit.wire, bit.offset);
	မဟုတ်ပါက
		*this = bit.data;
}

void DriveBit::merge(DriveBit const &other)
{
	အကယ်၍ (other.type_ == DriveType::NONE)
		ပြန်ပေး;
	အကယ်၍ (type_ == DriveType::NONE) {
		*this = other;
		ပြန်ပေး;
	}
	အကယ်၍ (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*this));
		*this = std::move(multi);
	}
	multiple().merge(other);
}


void DriveBitMultiple::merge(DriveBit const &single)
{
	အကယ်၍ (single.type() == DriveType::NONE)
		ပြန်ပေး;
	အကယ်၍ (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		ပြန်ပေး;
	}
	multiple_.emplace(single);
}

void DriveBitMultiple::merge(DriveBit &&single)
{
	အကယ်၍ (single.type() == DriveType::NONE)
		ပြန်ပေး;
	အကယ်၍ (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		ပြန်ပေး;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](ကိန်းပြည့် i) const
{
	DriveBitMultiple result;
	အတွက် (auto const &single : multiple_)
		result.merge(single[i]);
	ပြန်ပေး result;
}


bool DriveChunkWire::can_append(DriveBitWire const &bit) const
{
	ပြန်ပေး bit.wire == wire && bit.offset == offset + width;
}

bool DriveChunkWire::try_append(DriveBitWire const &bit)
{
	အကယ်၍ (!can_append(bit))
		ပြန်ပေး false;
	width += 1;
	ပြန်ပေး true;
}

bool DriveChunkWire::try_append(DriveChunkWire const &chunk)
{
	အကယ်၍ (chunk.wire != wire || chunk.offset != offset + width)
		ပြန်ပေး false;
	width += chunk.width;
	ပြန်ပေး true;
}

bool DriveChunkPort::can_append(DriveBitPort const &bit) const
{
	ပြန်ပေး bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

bool DriveChunkPort::try_append(DriveBitPort const &bit)
{
	အကယ်၍ (!can_append(bit))
		ပြန်ပေး false;
	width += 1;
	ပြန်ပေး true;
}

bool DriveChunkPort::try_append(DriveChunkPort const &chunk)
{
	အကယ်၍ (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		ပြန်ပေး false;
	width += chunk.width;
	ပြန်ပေး true;
}

bool DriveChunkMarker::can_append(DriveBitMarker const &bit) const
{
	ပြန်ပေး bit.marker == marker && bit.offset == offset + width;
}

bool DriveChunkMarker::try_append(DriveBitMarker const &bit)
{
	အကယ်၍ (!can_append(bit))
		ပြန်ပေး false;
	width += 1;
	ပြန်ပေး true;
}

bool DriveChunkMarker::try_append(DriveChunkMarker const &chunk)
{
	အကယ်၍ (chunk.marker != marker || chunk.offset != offset + width)
		ပြန်ပေး false;
	width += chunk.width;
	ပြန်ပေး true;
}


bool DriveChunkMultiple::can_append(DriveBitMultiple const &bit) const
{
	အကယ်၍ (bit.multiple().size() != multiple_.size())
		ပြန်ပေး false;

	ကိန်းပြည့် const_drivers = 0;
	အတွက် (DriveChunk const &single : multiple_)
		အကယ်၍ (single.is_constant())
			const_drivers += 1;

	အကယ်၍ (const_drivers > 1)
		ပြန်ပေး false;

	အတွက် (DriveBit const &single : bit.multiple())
		အကယ်၍ (single.is_constant())
			const_drivers -= 1;

	အကယ်၍ (const_drivers != 0)
		ပြန်ပေး false;

	အတွက် (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				အကယ်၍ (!bit.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				အကယ်၍ (!bit.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				အကယ်၍ (!bit.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			default:
				ပြန်ပေး false;
		}
	}

	ပြန်ပေး true;
}

bool DriveChunkMultiple::can_append(DriveChunkMultiple const &chunk) const
{
	အကယ်၍ (chunk.multiple().size() != multiple_.size())
		ပြန်ပေး false;

	ကိန်းပြည့် const_drivers = 0;
	အတွက် (DriveChunk const &single : multiple_)
		အကယ်၍ (single.is_constant())
			const_drivers += 1;

	အကယ်၍ (const_drivers > 1)
		ပြန်ပေး false;

	အတွက် (DriveChunk const &single : chunk.multiple())
		အကယ်၍ (single.is_constant())
			const_drivers -= 1;

	အကယ်၍ (const_drivers != 0)
		ပြန်ပေး false;

	အတွက် (DriveChunk const &single : multiple_)
	{
		switch (single.type())
		{
			pak_sa DriveType::CONSTANT: {
			} bha_nga;
			pak_sa DriveType::WIRE: {
				auto const &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				အကယ်၍ (!chunk.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			pak_sa DriveType::PORT: {
				auto const &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				အကယ်၍ (!chunk.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			pak_sa DriveType::MARKER: {
				auto const &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				အကယ်၍ (!chunk.multiple().count(next))
					ပြန်ပေး false;
			} bha_nga;
			default:
				ပြန်ပေး false;
		}
	}

	ပြန်ပေး true;
}

bool DriveChunkMultiple::try_append(DriveBitMultiple const &bit)
{
	အကယ်၍ (!can_append(bit))
		ပြန်ပေး false;
	width_ += 1;
	State constant;

	အတွက် (DriveBit const &single : bit.multiple())
		အကယ်၍ (single.is_constant())
			constant = single.constant();

	အတွက် (DriveChunk &single : multiple_)
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
	ပြန်ပေး true;
}

bool DriveChunkMultiple::try_append(DriveChunkMultiple const &chunk)
{
	အကယ်၍ (!can_append(chunk))
		ပြန်ပေး false;
	ကိန်းပြည့် width = chunk.size();
	width_ += width;
	Const constant;

	အတွက် (DriveChunk const &single : chunk.multiple())
		အကယ်၍ (single.is_constant())
			constant = single.constant();

	အတွက် (DriveChunk &single : multiple_)
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
	ပြန်ပေး true;
}

bool DriveChunk::can_append(DriveBit const &bit) const
{
	အကယ်၍ (size() == 0)
		ပြန်ပေး true;
	အကယ်၍ (bit.type() != type_)
		ပြန်ပေး false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			ပြန်ပေး true;
		pak_sa DriveType::CONSTANT:
			ပြန်ပေး true;
		pak_sa DriveType::WIRE:
			ပြန်ပေး wire_.can_append(bit.wire());
		pak_sa DriveType::PORT:
			ပြန်ပေး port_.can_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ပြန်ပေး multiple_.can_append(bit.multiple());
		default:
			log_abort();
	}
}

bool DriveChunk::try_append(DriveBit const &bit)
{
	အကယ်၍ (size() == 0)
		*this = bit;
	အကယ်၍ (bit.type() != type_)
		ပြန်ပေး false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += 1;
			ပြန်ပေး true;
		pak_sa DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			ပြန်ပေး true;
		pak_sa DriveType::WIRE:
			ပြန်ပေး wire_.try_append(bit.wire());
		pak_sa DriveType::PORT:
			ပြန်ပေး port_.try_append(bit.port());
		pak_sa DriveType::MULTIPLE:
			ပြန်ပေး multiple_.try_append(bit.multiple());
		default:
			log_abort();
	}
}


bool DriveChunk::try_append(DriveChunk const &chunk)
{
	အကယ်၍ (size() == 0)
		*this = chunk;
	အကယ်၍ (chunk.type_ != type_)
		ပြန်ပေး false;
	switch (type_)
	{
		pak_sa DriveType::NONE:
			none_ += chunk.none_;
			ပြန်ပေး true;
		pak_sa DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			ပြန်ပေး true;
		pak_sa DriveType::WIRE:
			ပြန်ပေး wire_.try_append(chunk.wire());
		pak_sa DriveType::PORT:
			ပြန်ပေး port_.try_append(chunk.port());
		pak_sa DriveType::MARKER:
			ပြန်ပေး marker_.try_append(chunk.marker());
		pak_sa DriveType::MULTIPLE:
			ပြန်ပေး multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

void DriveSpec::append(DriveBit const &bit)
{
	hash_ = 0;
	အကယ်၍ (!packed()) {
		bits_.push_back(bit);
		width_ += 1;
		ပြန်ပေး;
	}

	အကယ်၍ (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

void DriveSpec::append(DriveChunk const &chunk)
{
	hash_ = 0;
	pack();
	အကယ်၍ (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

void DriveSpec::pack() const {
	အကယ်၍ (bits_.empty())
		ပြန်ပေး;
	std::vector<DriveBit> bits(std::move(bits_));
	အတွက် (auto &bit : bits)
		အကယ်၍ (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

void DriveSpec::unpack() const {
	အကယ်၍ (chunks_.empty())
		ပြန်ပေး;
	အတွက် (auto &chunk : chunks_)
	{
		အတွက် (ကိန်းပြည့် i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

void DriveSpec::compute_width()
{
	width_ = 0;
	အတွက် (auto const &chunk : chunks_)
		width_ += chunk.size();
}

void DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	အကယ်၍ (first_edges.emplace(src, dst).first->second == dst)
		ပြန်ပေး;
	အကယ်၍ (second_edges.emplace(src, dst).first->second == dst)
		ပြန်ပေး;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::pop_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	အကယ်၍ (found_more != more_edges.end()) {
		auto result = found_more->second.pop();
		အကယ်၍ (found_more->second.empty())
			more_edges.erase(found_more);
		ပြန်ပေး result;
	}

	auto found_second = second_edges.find(src);
	အကယ်၍ (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		ပြန်ပေး result;
	}

	auto found_first = first_edges.find(src);
	အကယ်၍ (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		ပြန်ပေး result;
	}

	ပြန်ပေး DriveBitId();
}

void DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

bool DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	ပြန်ပေး first_edges.count(src);
}

ကိန်းပြည့် DriverMap::DriveBitGraph::count(DriveBitId src)
{
	အကယ်၍ (!first_edges.count(src))
		ပြန်ပေး 0;
	အကယ်၍ (!second_edges.count(src))
		ပြန်ပေး 1;
	auto found = more_edges.find(src);
	အကယ်၍ (found == more_edges.end())
		ပြန်ပေး 2;
	ပြန်ပေး GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, ကိန်းပြည့် index)
{
	အကယ်၍ (index == 0)
		ပြန်ပေး first_edges.at(src);
	မဟုတ်ပါက အကယ်၍ (index == 1)
		ပြန်ပေး second_edges.at(src);
	မဟုတ်ပါက
		ပြန်ပေး *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit const &bit)
{
	switch (bit.type())
	{
		pak_sa DriveType::NONE:
			ပြန်ပေး BitMode::NONE;
		pak_sa DriveType::CONSTANT:
			// TODO how to handle Sx here?
			ပြန်ပေး bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		pak_sa DriveType::WIRE: {
			auto const &wire = bit.wire();
			bool driver = wire.wire->port_input;
			bool driven = wire.wire->port_output;

			အကယ်၍ (driver && !driven)
				ပြန်ပေး BitMode::DRIVER;
			မဟုတ်ပါက အကယ်၍ (driven && !driver)
				ပြန်ပေး BitMode::DRIVEN;
			မဟုတ်ပါက အကယ်၍ (driver && driven)
				ပြန်ပေး BitMode::TRISTATE;
			မဟုတ်ပါက
				ပြန်ပေး keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		pak_sa DriveType::PORT: {
			auto const &port = bit.port();
			bool driver = celltypes.cell_output(port.cell->type, port.port);
			bool driven = celltypes.cell_input(port.cell->type, port.port);
			အကယ်၍ (driver && !driven)
				ပြန်ပေး BitMode::DRIVER;
			မဟုတ်ပါက အကယ်၍ (driven && !driver)
				ပြန်ပေး BitMode::DRIVEN_UNIQUE;
			မဟုတ်ပါက
				ပြန်ပေး BitMode::TRISTATE;
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
			ပြန်ပေး -1;
		pak_sa DriveType::CONSTANT:
			ပြန်ပေး (ကိန်းပြည့်)bit.constant();
		pak_sa DriveType::WIRE: {
			auto const &wire_bit = bit.wire();
			ကိန်းပြည့် offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			အကယ်၍ (insertion.second) {
				အကယ်၍ (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} မဟုတ်ပါက
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			ပြန်ပေး insertion.first->second.id + wire_bit.offset;
		}
		pak_sa DriveType::PORT: {
			auto const &port_bit = bit.port();
			auto key = std::make_pair(port_bit.cell, port_bit.port);
			ကိန်းပြည့် offset = next_offset;
			auto insertion = port_offsets.emplace(key, offset);
			အကယ်၍ (insertion.second) {
				ကိန်းပြည့် width = port_bit.cell->connections().at(port_bit.port).size();
				အကယ်၍ (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} မဟုတ်ပါက
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			ပြန်ပေး insertion.first->second.id + port_bit.offset;
		}
		default:
			log_assert(false && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	အကယ်၍ (found_isolated != isolated_drive_bits.end())
		ပြန်ပေး found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	အကယ်၍ (found == drive_bits.begin()) {
		ပြန်ပေး id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	အကယ်၍ (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} မဟုတ်ပါက {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	ပြန်ပေး result;
}

void DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	အကယ်၍ (driven_id == driver_id)
		ပြန်ပေး;

	same_driver.merge(driven_id, driver_id);

	အတွက် (ကိန်းပြည့် i = 0, end = connected_drivers.count(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	အတွက် (ကိန်းပြည့် i = 0, end = connected_undirected.count(driven_id); i != end; ++i)
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
	အတွက် (auto const &conn : module->connections())
		add(conn.first, conn.second);

	အတွက် (auto cell : module->cells())
		အတွက် (auto const &conn : cell->connections())
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

	အကယ်၍ (a_id == b_id)
		ပြန်ပေး;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	အကယ်၍ (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	မဟုတ်ပါက အကယ်၍ (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	မဟုတ်ပါက အကယ်၍ (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	မဟုတ်ပါက အကယ်၍ (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	မဟုတ်ပါက အကယ်၍ (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	မဟုတ်ပါက အကယ်၍ (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	မဟုတ်ပါက
		connect_undirected(a_id, b_id);

	ပြန်ပေး;
}

// Specialized version that avoids unpacking
void DriverMap::add(SigSpec const &a, SigSpec const &b)
{
	log_assert(a.size() == b.size());
	auto const &a_chunks = a.chunks();
	auto const &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	ကိန်းပြည့် a_offset = 0;

	auto b_chunk = b_chunks.begin();
	ကိန်းပြည့် b_offset = 0;

	SigChunk tmp_a, tmp_b;
	နေစဉ် (a_chunk != a_end) {
		ကိန်းပြည့် a_width = a_chunk->width - a_offset;
		အကယ်၍ (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			continue;
		}
		ကိန်းပြည့် b_width = b_chunk->width - b_offset;
		အကယ်၍ (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			continue;
		}
		ကိန်းပြည့် width = std::min(a_width, b_width);
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
	ကိန်းပြည့် offset = 0;
	အတွက် (auto const &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

void DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, ကိန်းပြည့်> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	အတွက် (ကိန်းပြည့် pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		အကယ်၍ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		အကယ်၍ (connected_drivers.contains(current))
			drivers.emplace(current);

		ကိန်းပြည့် undirected_driver_count = connected_undirected.count(current);

		အတွက် (ကိန်းပြည့် i = 0; i != undirected_driver_count; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	အကယ်၍ (drivers.empty())
		အတွက် (auto seen_id : seen)
			drivers.emplace(seen_id);

	အတွက် (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		အတွက် (ကိန်းပြည့် pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			ကိန်းပြည့် undirected_driver_count = connected_undirected.count(current);

			အတွက် (ကိန်းပြည့် i = 0; i != undirected_driver_count; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				အကယ်၍ (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	အတွက် (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit const &bit)
{
	အကယ်၍ (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		ပြန်ပေး bit;
	အကယ်၍ (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		အတွက် (auto const &inner : bit.multiple().multiple())
			result.merge((*this)(inner));
		ပြန်ပေး result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	အကယ်၍ (mode == BitMode::KEEP && bit_repr_id != bit_id)
		ပြန်ပေး bit_repr;

	ကိန်းပြည့် implicit_driver_count = connected_drivers.count(bit_repr_id);
	အကယ်၍ (connected_undirected.contains(bit_repr_id) && !oriented_present.count(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	အကယ်၍ (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	အတွက် (ကိန်းပြည့် i = 0; i != implicit_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	ကိန်းပြည့် oriented_driver_count = connected_oriented.count(bit_repr_id);
	အတွက် (ကိန်းပြည့် i = 0; i != oriented_driver_count; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	ပြန်ပေး driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	အတွက် (ကိန်းပြည့် i = 0, width = spec.size(); i != width; ++i)
		result.append((*this)(spec[i]));

	ပြန်ပေး result;
}

std::string log_signal(DriveChunkWire const &chunk)
{
	std::string id = chunk.wire->name.unescape();
	အကယ်၍ (chunk.is_whole())
		ပြန်ပေး id;
	အကယ်၍ (chunk.width == 1)
		ပြန်ပေး stringf("%s [%d]", id, chunk.offset);
	ပြန်ပေး stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort const &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	အကယ်၍ (chunk.is_whole())
		ပြန်ပေး stringf("%s <%s>", cell_id, port_id);
	အကယ်၍ (chunk.width == 1)
		ပြန်ပေး stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	ပြန်ပေး stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker const &chunk)
{
	အကယ်၍ (chunk.width == 1)
		ပြန်ပေး stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	ပြန်ပေး stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk const &chunk)
{
	switch (chunk.type())
	{
		pak_sa DriveType::NONE:
			ပြန်ပေး stringf("<none x%d>", chunk.size());
		pak_sa DriveType::CONSTANT:
			ပြန်ပေး log_const(chunk.constant());
		pak_sa DriveType::WIRE:
			ပြန်ပေး log_signal(chunk.wire());
		pak_sa DriveType::PORT:
			ပြန်ပေး log_signal(chunk.port());
		pak_sa DriveType::MARKER:
			ပြန်ပေး log_signal(chunk.marker());
		pak_sa DriveType::MULTIPLE: {
			std::string str = "<multiple";
			const var_na *sep = " ";
			အတွက် (auto const &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			ပြန်ပေး str;
		}
		default:
			log_abort();
	}
}

std::string log_signal(DriveSpec const &spec)
{
	auto &chunks = spec.chunks();
	အကယ်၍ (chunks.empty())
		ပြန်ပေး "{}";
	အကယ်၍ (chunks.size() == 1)
		ပြန်ပေး log_signal(chunks[0]);

	std::string str;
	const var_na *sep = "{ ";

	အတွက် (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	ပြန်ပေး str;
}

YOSYS_NAMESPACE_END
