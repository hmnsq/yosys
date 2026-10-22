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

#அந்தர்பாவ "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit மாறிலி &bit)
{
	எனில் (bit.is_wire())
		*இது = DriveBitWire(bit.wire, bit.offset);
	இல்லையெனில்
		*இது = bit.data;
}

வெறுமை DriveBit::merge(DriveBit மாறிலி &other)
{
	எனில் (other.type_ == DriveType::NONE)
		திரும்பு;
	எனில் (type_ == DriveType::NONE) {
		*இது = other;
		திரும்பு;
	}
	எனில் (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*இது));
		*இது = std::move(multi);
	}
	multiple().merge(other);
}


வெறுமை DriveBitMultiple::merge(DriveBit மாறிலி &single)
{
	எனில் (single.type() == DriveType::NONE)
		திரும்பு;
	எனில் (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		திரும்பு;
	}
	multiple_.emplace(single);
}

வெறுமை DriveBitMultiple::merge(DriveBit &&single)
{
	எனில் (single.type() == DriveType::NONE)
		திரும்பு;
	எனில் (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		திரும்பு;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](முழு i) மாறிலி
{
	DriveBitMultiple result;
	க்கு (auto மாறிலி &single : multiple_)
		result.merge(single[i]);
	திரும்பு result;
}


தருக்க DriveChunkWire::can_append(DriveBitWire மாறிலி &bit) மாறிலி
{
	திரும்பு bit.wire == wire && bit.offset == offset + width;
}

தருக்க DriveChunkWire::try_append(DriveBitWire மாறிலி &bit)
{
	எனில் (!can_append(bit))
		திரும்பு பொய்;
	width += 1;
	திரும்பு உண்மை;
}

தருக்க DriveChunkWire::try_append(DriveChunkWire மாறிலி &chunk)
{
	எனில் (chunk.wire != wire || chunk.offset != offset + width)
		திரும்பு பொய்;
	width += chunk.width;
	திரும்பு உண்மை;
}

தருக்க DriveChunkPort::can_append(DriveBitPort மாறிலி &bit) மாறிலி
{
	திரும்பு bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

தருக்க DriveChunkPort::try_append(DriveBitPort மாறிலி &bit)
{
	எனில் (!can_append(bit))
		திரும்பு பொய்;
	width += 1;
	திரும்பு உண்மை;
}

தருக்க DriveChunkPort::try_append(DriveChunkPort மாறிலி &chunk)
{
	எனில் (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		திரும்பு பொய்;
	width += chunk.width;
	திரும்பு உண்மை;
}

தருக்க DriveChunkMarker::can_append(DriveBitMarker மாறிலி &bit) மாறிலி
{
	திரும்பு bit.marker == marker && bit.offset == offset + width;
}

தருக்க DriveChunkMarker::try_append(DriveBitMarker மாறிலி &bit)
{
	எனில் (!can_append(bit))
		திரும்பு பொய்;
	width += 1;
	திரும்பு உண்மை;
}

தருக்க DriveChunkMarker::try_append(DriveChunkMarker மாறிலி &chunk)
{
	எனில் (chunk.marker != marker || chunk.offset != offset + width)
		திரும்பு பொய்;
	width += chunk.width;
	திரும்பு உண்மை;
}


தருக்க DriveChunkMultiple::can_append(DriveBitMultiple மாறிலி &bit) மாறிலி
{
	எனில் (bit.multiple().size() != multiple_.size())
		திரும்பு பொய்;

	முழு const_drivers = 0;
	க்கு (DriveChunk மாறிலி &single : multiple_)
		எனில் (single.is_constant())
			const_drivers += 1;

	எனில் (const_drivers > 1)
		திரும்பு பொய்;

	க்கு (DriveBit மாறிலி &single : bit.multiple())
		எனில் (single.is_constant())
			const_drivers -= 1;

	எனில் (const_drivers != 0)
		திரும்பு பொய்;

	க்கு (DriveChunk மாறிலி &single : multiple_)
	{
		தேர்வு (single.type())
		{
			நிலை DriveType::CONSTANT: {
			} நிறுத்து;
			நிலை DriveType::WIRE: {
				auto மாறிலி &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				எனில் (!bit.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			நிலை DriveType::PORT: {
				auto மாறிலி &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				எனில் (!bit.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			நிலை DriveType::MARKER: {
				auto மாறிலி &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				எனில் (!bit.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			இயல்பு:
				திரும்பு பொய்;
		}
	}

	திரும்பு உண்மை;
}

தருக்க DriveChunkMultiple::can_append(DriveChunkMultiple மாறிலி &chunk) மாறிலி
{
	எனில் (chunk.multiple().size() != multiple_.size())
		திரும்பு பொய்;

	முழு const_drivers = 0;
	க்கு (DriveChunk மாறிலி &single : multiple_)
		எனில் (single.is_constant())
			const_drivers += 1;

	எனில் (const_drivers > 1)
		திரும்பு பொய்;

	க்கு (DriveChunk மாறிலி &single : chunk.multiple())
		எனில் (single.is_constant())
			const_drivers -= 1;

	எனில் (const_drivers != 0)
		திரும்பு பொய்;

	க்கு (DriveChunk மாறிலி &single : multiple_)
	{
		தேர்வு (single.type())
		{
			நிலை DriveType::CONSTANT: {
			} நிறுத்து;
			நிலை DriveType::WIRE: {
				auto மாறிலி &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				எனில் (!chunk.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			நிலை DriveType::PORT: {
				auto மாறிலி &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				எனில் (!chunk.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			நிலை DriveType::MARKER: {
				auto மாறிலி &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				எனில் (!chunk.multiple().மொத்தம்(next))
					திரும்பு பொய்;
			} நிறுத்து;
			இயல்பு:
				திரும்பு பொய்;
		}
	}

	திரும்பு உண்மை;
}

தருக்க DriveChunkMultiple::try_append(DriveBitMultiple மாறிலி &bit)
{
	எனில் (!can_append(bit))
		திரும்பு பொய்;
	width_ += 1;
	State constant;

	க்கு (DriveBit மாறிலி &single : bit.multiple())
		எனில் (single.is_constant())
			constant = single.constant();

	க்கு (DriveChunk &single : multiple_)
	{
		தேர்வு (single.type())
		{
			நிலை DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} நிறுத்து;
			நிலை DriveType::WIRE: {
				single.wire().width += 1;
			} நிறுத்து;
			நிலை DriveType::PORT: {
				single.port().width += 1;
			} நிறுத்து;
			நிலை DriveType::MARKER: {
				single.marker().width += 1;
			} நிறுத்து;
			இயல்பு:
				log_abort();
		}
	}
	திரும்பு உண்மை;
}

தருக்க DriveChunkMultiple::try_append(DriveChunkMultiple மாறிலி &chunk)
{
	எனில் (!can_append(chunk))
		திரும்பு பொய்;
	முழு width = chunk.size();
	width_ += width;
	Const constant;

	க்கு (DriveChunk மாறிலி &single : chunk.multiple())
		எனில் (single.is_constant())
			constant = single.constant();

	க்கு (DriveChunk &single : multiple_)
	{
		தேர்வு (single.type())
		{
			நிலை DriveType::CONSTANT: {
				single.constant().append(constant);
			} நிறுத்து;
			நிலை DriveType::WIRE: {
				single.wire().width += width;
			} நிறுத்து;
			நிலை DriveType::PORT: {
				single.port().width += width;
			} நிறுத்து;
			நிலை DriveType::MARKER: {
				single.marker().width += width;
			} நிறுத்து;
			இயல்பு:
				log_abort();
		}
	}
	திரும்பு உண்மை;
}

தருக்க DriveChunk::can_append(DriveBit மாறிலி &bit) மாறிலி
{
	எனில் (size() == 0)
		திரும்பு உண்மை;
	எனில் (bit.type() != type_)
		திரும்பு பொய்;
	தேர்வு (type_)
	{
		நிலை DriveType::NONE:
			திரும்பு உண்மை;
		நிலை DriveType::CONSTANT:
			திரும்பு உண்மை;
		நிலை DriveType::WIRE:
			திரும்பு wire_.can_append(bit.wire());
		நிலை DriveType::PORT:
			திரும்பு port_.can_append(bit.port());
		நிலை DriveType::MULTIPLE:
			திரும்பு multiple_.can_append(bit.multiple());
		இயல்பு:
			log_abort();
	}
}

தருக்க DriveChunk::try_append(DriveBit மாறிலி &bit)
{
	எனில் (size() == 0)
		*இது = bit;
	எனில் (bit.type() != type_)
		திரும்பு பொய்;
	தேர்வு (type_)
	{
		நிலை DriveType::NONE:
			none_ += 1;
			திரும்பு உண்மை;
		நிலை DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			திரும்பு உண்மை;
		நிலை DriveType::WIRE:
			திரும்பு wire_.try_append(bit.wire());
		நிலை DriveType::PORT:
			திரும்பு port_.try_append(bit.port());
		நிலை DriveType::MULTIPLE:
			திரும்பு multiple_.try_append(bit.multiple());
		இயல்பு:
			log_abort();
	}
}


தருக்க DriveChunk::try_append(DriveChunk மாறிலி &chunk)
{
	எனில் (size() == 0)
		*இது = chunk;
	எனில் (chunk.type_ != type_)
		திரும்பு பொய்;
	தேர்வு (type_)
	{
		நிலை DriveType::NONE:
			none_ += chunk.none_;
			திரும்பு உண்மை;
		நிலை DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			திரும்பு உண்மை;
		நிலை DriveType::WIRE:
			திரும்பு wire_.try_append(chunk.wire());
		நிலை DriveType::PORT:
			திரும்பு port_.try_append(chunk.port());
		நிலை DriveType::MARKER:
			திரும்பு marker_.try_append(chunk.marker());
		நிலை DriveType::MULTIPLE:
			திரும்பு multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

வெறுமை DriveSpec::append(DriveBit மாறிலி &bit)
{
	hash_ = 0;
	எனில் (!packed()) {
		bits_.தள்ளு_back(bit);
		width_ += 1;
		திரும்பு;
	}

	எனில் (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

வெறுமை DriveSpec::append(DriveChunk மாறிலி &chunk)
{
	hash_ = 0;
	pack();
	எனில் (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

வெறுமை DriveSpec::pack() மாறிலி {
	எனில் (bits_.empty())
		திரும்பு;
	std::vector<DriveBit> bits(std::move(bits_));
	க்கு (auto &bit : bits)
		எனில் (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

வெறுமை DriveSpec::unpack() மாறிலி {
	எனில் (chunks_.empty())
		திரும்பு;
	க்கு (auto &chunk : chunks_)
	{
		க்கு (முழு i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

வெறுமை DriveSpec::compute_width()
{
	width_ = 0;
	க்கு (auto மாறிலி &chunk : chunks_)
		width_ += chunk.size();
}

வெறுமை DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	எனில் (first_edges.emplace(src, dst).first->second == dst)
		திரும்பு;
	எனில் (second_edges.emplace(src, dst).first->second == dst)
		திரும்பு;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::எடு_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	எனில் (found_more != more_edges.end()) {
		auto result = found_more->second.எடு();
		எனில் (found_more->second.empty())
			more_edges.erase(found_more);
		திரும்பு result;
	}

	auto found_second = second_edges.find(src);
	எனில் (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		திரும்பு result;
	}

	auto found_first = first_edges.find(src);
	எனில் (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		திரும்பு result;
	}

	திரும்பு DriveBitId();
}

வெறுமை DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

தருக்க DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	திரும்பு first_edges.மொத்தம்(src);
}

முழு DriverMap::DriveBitGraph::மொத்தம்(DriveBitId src)
{
	எனில் (!first_edges.மொத்தம்(src))
		திரும்பு 0;
	எனில் (!second_edges.மொத்தம்(src))
		திரும்பு 1;
	auto found = more_edges.find(src);
	எனில் (found == more_edges.end())
		திரும்பு 2;
	திரும்பு GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, முழு index)
{
	எனில் (index == 0)
		திரும்பு first_edges.at(src);
	இல்லையெனில் எனில் (index == 1)
		திரும்பு second_edges.at(src);
	இல்லையெனில்
		திரும்பு *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit மாறிலி &bit)
{
	தேர்வு (bit.type())
	{
		நிலை DriveType::NONE:
			திரும்பு BitMode::NONE;
		நிலை DriveType::CONSTANT:
			// TODO how to handle Sx here?
			திரும்பு bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		நிலை DriveType::WIRE: {
			auto மாறிலி &wire = bit.wire();
			தருக்க driver = wire.wire->port_உள்ளீடு;
			தருக்க driven = wire.wire->port_output;

			எனில் (driver && !driven)
				திரும்பு BitMode::DRIVER;
			இல்லையெனில் எனில் (driven && !driver)
				திரும்பு BitMode::DRIVEN;
			இல்லையெனில் எனில் (driver && driven)
				திரும்பு BitMode::TRISTATE;
			இல்லையெனில்
				திரும்பு keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		நிலை DriveType::PORT: {
			auto மாறிலி &port = bit.port();
			தருக்க driver = celltypes.cell_output(port.cell->type, port.port);
			தருக்க driven = celltypes.cell_உள்ளீடு(port.cell->type, port.port);
			எனில் (driver && !driven)
				திரும்பு BitMode::DRIVER;
			இல்லையெனில் எனில் (driven && !driver)
				திரும்பு BitMode::DRIVEN_UNIQUE;
			இல்லையெனில்
				திரும்பு BitMode::TRISTATE;
		}
		நிலை DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		இயல்பு:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit மாறிலி &bit)
{
	தேர்வு (bit.type())
	{
		நிலை DriveType::NONE:
			திரும்பு -1;
		நிலை DriveType::CONSTANT:
			திரும்பு (முழு)bit.constant();
		நிலை DriveType::WIRE: {
			auto மாறிலி &wire_bit = bit.wire();
			முழு offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			எனில் (insertion.second) {
				எனில் (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} இல்லையெனில்
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			திரும்பு insertion.first->second.id + wire_bit.offset;
		}
		நிலை DriveType::PORT: {
			auto மாறிலி &port_bit = bit.port();
			auto சாவி = std::make_pair(port_bit.cell, port_bit.port);
			முழு offset = next_offset;
			auto insertion = port_offsets.emplace(சாவி, offset);
			எனில் (insertion.second) {
				முழு width = port_bit.cell->connections().at(port_bit.port).size();
				எனில் (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} இல்லையெனில்
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			திரும்பு insertion.first->second.id + port_bit.offset;
		}
		இயல்பு:
			log_assert(பொய் && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	எனில் (found_isolated != isolated_drive_bits.end())
		திரும்பு found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	எனில் (found == drive_bits.begin()) {
		திரும்பு id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	எனில் (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} இல்லையெனில் {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	திரும்பு result;
}

வெறுமை DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	எனில் (driven_id == driver_id)
		திரும்பு;

	same_driver.merge(driven_id, driver_id);

	க்கு (முழு i = 0, end = connected_drivers.மொத்தம்(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	க்கு (முழு i = 0, end = connected_undirected.மொத்தம்(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

வெறுமை DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

வெறுமை DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

வெறுமை DriverMap::add(Module *module)
{
	க்கு (auto மாறிலி &conn : module->connections())
		add(conn.first, conn.second);

	க்கு (auto cell : module->cells())
		க்கு (auto மாறிலி &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
வெறுமை DriverMap::add(DriveBit மாறிலி &a, DriveBit மாறிலி &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	எனில் (a_id == b_id)
		திரும்பு;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	எனில் (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	இல்லையெனில் எனில் (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	இல்லையெனில் எனில் (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	இல்லையெனில் எனில் (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	இல்லையெனில் எனில் (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	இல்லையெனில் எனில் (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	இல்லையெனில்
		connect_undirected(a_id, b_id);

	திரும்பு;
}

// Specialized version that avoids unpacking
வெறுமை DriverMap::add(SigSpec மாறிலி &a, SigSpec மாறிலி &b)
{
	log_assert(a.size() == b.size());
	auto மாறிலி &a_chunks = a.chunks();
	auto மாறிலி &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	முழு a_offset = 0;

	auto b_chunk = b_chunks.begin();
	முழு b_offset = 0;

	SigChunk tmp_a, tmp_b;
	போது (a_chunk != a_end) {
		முழு a_width = a_chunk->width - a_offset;
		எனில் (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			தொடர்;
		}
		முழு b_width = b_chunk->width - b_offset;
		எனில் (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			தொடர்;
		}
		முழு width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk மாறிலி &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk மாறிலி &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

வெறுமை DriverMap::add_port(Cell *cell, IdString port, SigSpec மாறிலி &b)
{
	முழு offset = 0;
	க்கு (auto மாறிலி &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

வெறுமை DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, முழு> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	க்கு (முழு pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		எனில் (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		எனில் (connected_drivers.contains(current))
			drivers.emplace(current);

		முழு undirected_driver_மொத்தம் = connected_undirected.மொத்தம்(current);

		க்கு (முழு i = 0; i != undirected_driver_மொத்தம்; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	எனில் (drivers.empty())
		க்கு (auto seen_id : seen)
			drivers.emplace(seen_id);

	க்கு (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		க்கு (முழு pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			முழு undirected_driver_மொத்தம் = connected_undirected.மொத்தம்(current);

			க்கு (முழு i = 0; i != undirected_driver_மொத்தம்; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				எனில் (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	க்கு (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit மாறிலி &bit)
{
	எனில் (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		திரும்பு bit;
	எனில் (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		க்கு (auto மாறிலி &inner : bit.multiple().multiple())
			result.merge((*இது)(inner));
		திரும்பு result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	எனில் (mode == BitMode::KEEP && bit_repr_id != bit_id)
		திரும்பு bit_repr;

	முழு implicit_driver_மொத்தம் = connected_drivers.மொத்தம்(bit_repr_id);
	எனில் (connected_undirected.contains(bit_repr_id) && !oriented_present.மொத்தம்(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	எனில் (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	க்கு (முழு i = 0; i != implicit_driver_மொத்தம்; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	முழு oriented_driver_மொத்தம் = connected_oriented.மொத்தம்(bit_repr_id);
	க்கு (முழு i = 0; i != oriented_driver_மொத்தம்; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	திரும்பு driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	க்கு (முழு i = 0, width = spec.size(); i != width; ++i)
		result.append((*இது)(spec[i]));

	திரும்பு result;
}

std::string log_signal(DriveChunkWire மாறிலி &chunk)
{
	std::string id = chunk.wire->name.unescape();
	எனில் (chunk.is_whole())
		திரும்பு id;
	எனில் (chunk.width == 1)
		திரும்பு stringf("%s [%d]", id, chunk.offset);
	திரும்பு stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort மாறிலி &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	எனில் (chunk.is_whole())
		திரும்பு stringf("%s <%s>", cell_id, port_id);
	எனில் (chunk.width == 1)
		திரும்பு stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	திரும்பு stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker மாறிலி &chunk)
{
	எனில் (chunk.width == 1)
		திரும்பு stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	திரும்பு stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk மாறிலி &chunk)
{
	தேர்வு (chunk.type())
	{
		நிலை DriveType::NONE:
			திரும்பு stringf("<none x%d>", chunk.size());
		நிலை DriveType::CONSTANT:
			திரும்பு log_const(chunk.constant());
		நிலை DriveType::WIRE:
			திரும்பு log_signal(chunk.wire());
		நிலை DriveType::PORT:
			திரும்பு log_signal(chunk.port());
		நிலை DriveType::MARKER:
			திரும்பு log_signal(chunk.marker());
		நிலை DriveType::MULTIPLE: {
			std::string str = "<multiple";
			மாறிலி எழுத்து *sep = " ";
			க்கு (auto மாறிலி &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			திரும்பு str;
		}
		இயல்பு:
			log_abort();
	}
}

std::string log_signal(DriveSpec மாறிலி &spec)
{
	auto &chunks = spec.chunks();
	எனில் (chunks.empty())
		திரும்பு "{}";
	எனில் (chunks.size() == 1)
		திரும்பு log_signal(chunks[0]);

	std::string str;
	மாறிலி எழுத்து *sep = "{ ";

	க்கு (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	திரும்பு str;
}

YOSYS_NAMESPACE_END
