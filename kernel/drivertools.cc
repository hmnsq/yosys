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

#شامل "kernel/drivertools.h"

YOSYS_NAMESPACE_BEGIN

DriveBit::DriveBit(SigBit ثابت &bit)
{
	اگر (bit.is_wire())
		*این = DriveBitWire(bit.wire, bit.offset);
	وگرنه
		*این = bit.data;
}

تهی DriveBit::merge(DriveBit ثابت &other)
{
	اگر (other.type_ == DriveType::NONE)
		بازگشت;
	اگر (type_ == DriveType::NONE) {
		*این = other;
		بازگشت;
	}
	اگر (type_ != DriveType::MULTIPLE) {
		DriveBitMultiple multi(std::move(*این));
		*این = std::move(multi);
	}
	multiple().merge(other);
}


تهی DriveBitMultiple::merge(DriveBit ثابت &single)
{
	اگر (single.type() == DriveType::NONE)
		بازگشت;
	اگر (single.type() == DriveType::MULTIPLE) {
		merge(single.multiple());
		بازگشت;
	}
	multiple_.emplace(single);
}

تهی DriveBitMultiple::merge(DriveBit &&single)
{
	اگر (single.type() == DriveType::NONE)
		بازگشت;
	اگر (single.type() == DriveType::MULTIPLE) {
		merge(std::move(single.multiple()));
		بازگشت;
	}
	multiple_.emplace(std::move(single));
}

DriveBitMultiple DriveChunkMultiple::operator[](صحیح i) ثابت
{
	DriveBitMultiple result;
	برای (auto ثابت &single : multiple_)
		result.merge(single[i]);
	بازگشت result;
}


منطقی DriveChunkWire::can_append(DriveBitWire ثابت &bit) ثابت
{
	بازگشت bit.wire == wire && bit.offset == offset + width;
}

منطقی DriveChunkWire::try_append(DriveBitWire ثابت &bit)
{
	اگر (!can_append(bit))
		بازگشت نادرست;
	width += 1;
	بازگشت درست;
}

منطقی DriveChunkWire::try_append(DriveChunkWire ثابت &chunk)
{
	اگر (chunk.wire != wire || chunk.offset != offset + width)
		بازگشت نادرست;
	width += chunk.width;
	بازگشت درست;
}

منطقی DriveChunkPort::can_append(DriveBitPort ثابت &bit) ثابت
{
	بازگشت bit.cell == cell && bit.port == port && bit.offset == offset + width;
}

منطقی DriveChunkPort::try_append(DriveBitPort ثابت &bit)
{
	اگر (!can_append(bit))
		بازگشت نادرست;
	width += 1;
	بازگشت درست;
}

منطقی DriveChunkPort::try_append(DriveChunkPort ثابت &chunk)
{
	اگر (chunk.cell != cell || chunk.port != port || chunk.offset != offset + width)
		بازگشت نادرست;
	width += chunk.width;
	بازگشت درست;
}

منطقی DriveChunkMarker::can_append(DriveBitMarker ثابت &bit) ثابت
{
	بازگشت bit.marker == marker && bit.offset == offset + width;
}

منطقی DriveChunkMarker::try_append(DriveBitMarker ثابت &bit)
{
	اگر (!can_append(bit))
		بازگشت نادرست;
	width += 1;
	بازگشت درست;
}

منطقی DriveChunkMarker::try_append(DriveChunkMarker ثابت &chunk)
{
	اگر (chunk.marker != marker || chunk.offset != offset + width)
		بازگشت نادرست;
	width += chunk.width;
	بازگشت درست;
}


منطقی DriveChunkMultiple::can_append(DriveBitMultiple ثابت &bit) ثابت
{
	اگر (bit.multiple().size() != multiple_.size())
		بازگشت نادرست;

	صحیح const_drivers = 0;
	برای (DriveChunk ثابت &single : multiple_)
		اگر (single.is_constant())
			const_drivers += 1;

	اگر (const_drivers > 1)
		بازگشت نادرست;

	برای (DriveBit ثابت &single : bit.multiple())
		اگر (single.is_constant())
			const_drivers -= 1;

	اگر (const_drivers != 0)
		بازگشت نادرست;

	برای (DriveChunk ثابت &single : multiple_)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
			} بشکن;
			حالت DriveType::WIRE: {
				auto ثابت &wire = single.wire();
				DriveBit next = DriveBitWire(wire.wire, wire.offset + wire.width);
				اگر (!bit.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			حالت DriveType::PORT: {
				auto ثابت &port = single.port();
				DriveBit next = DriveBitPort(port.cell, port.port, port.offset + port.width);
				اگر (!bit.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			حالت DriveType::MARKER: {
				auto ثابت &marker = single.marker();
				DriveBit next = DriveBitMarker(marker.marker, marker.offset + marker.width);
				اگر (!bit.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			پیشفرض:
				بازگشت نادرست;
		}
	}

	بازگشت درست;
}

منطقی DriveChunkMultiple::can_append(DriveChunkMultiple ثابت &chunk) ثابت
{
	اگر (chunk.multiple().size() != multiple_.size())
		بازگشت نادرست;

	صحیح const_drivers = 0;
	برای (DriveChunk ثابت &single : multiple_)
		اگر (single.is_constant())
			const_drivers += 1;

	اگر (const_drivers > 1)
		بازگشت نادرست;

	برای (DriveChunk ثابت &single : chunk.multiple())
		اگر (single.is_constant())
			const_drivers -= 1;

	اگر (const_drivers != 0)
		بازگشت نادرست;

	برای (DriveChunk ثابت &single : multiple_)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
			} بشکن;
			حالت DriveType::WIRE: {
				auto ثابت &wire = single.wire();
				DriveChunk next = DriveChunkWire(wire.wire, wire.offset + wire.width, chunk.size());
				اگر (!chunk.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			حالت DriveType::PORT: {
				auto ثابت &port = single.port();
				DriveChunk next = DriveChunkPort(port.cell, port.port, port.offset + port.width, chunk.size());
				اگر (!chunk.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			حالت DriveType::MARKER: {
				auto ثابت &marker = single.marker();
				DriveChunk next = DriveChunkMarker(marker.marker, marker.offset + marker.width, chunk.size());
				اگر (!chunk.multiple().شمار(next))
					بازگشت نادرست;
			} بشکن;
			پیشفرض:
				بازگشت نادرست;
		}
	}

	بازگشت درست;
}

منطقی DriveChunkMultiple::try_append(DriveBitMultiple ثابت &bit)
{
	اگر (!can_append(bit))
		بازگشت نادرست;
	width_ += 1;
	State constant;

	برای (DriveBit ثابت &single : bit.multiple())
		اگر (single.is_constant())
			constant = single.constant();

	برای (DriveChunk &single : multiple_)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
				single.constant().append(RTLIL::Const(constant));
			} بشکن;
			حالت DriveType::WIRE: {
				single.wire().width += 1;
			} بشکن;
			حالت DriveType::PORT: {
				single.port().width += 1;
			} بشکن;
			حالت DriveType::MARKER: {
				single.marker().width += 1;
			} بشکن;
			پیشفرض:
				log_abort();
		}
	}
	بازگشت درست;
}

منطقی DriveChunkMultiple::try_append(DriveChunkMultiple ثابت &chunk)
{
	اگر (!can_append(chunk))
		بازگشت نادرست;
	صحیح width = chunk.size();
	width_ += width;
	Const constant;

	برای (DriveChunk ثابت &single : chunk.multiple())
		اگر (single.is_constant())
			constant = single.constant();

	برای (DriveChunk &single : multiple_)
	{
		انتخاب (single.type())
		{
			حالت DriveType::CONSTANT: {
				single.constant().append(constant);
			} بشکن;
			حالت DriveType::WIRE: {
				single.wire().width += width;
			} بشکن;
			حالت DriveType::PORT: {
				single.port().width += width;
			} بشکن;
			حالت DriveType::MARKER: {
				single.marker().width += width;
			} بشکن;
			پیشفرض:
				log_abort();
		}
	}
	بازگشت درست;
}

منطقی DriveChunk::can_append(DriveBit ثابت &bit) ثابت
{
	اگر (size() == 0)
		بازگشت درست;
	اگر (bit.type() != type_)
		بازگشت نادرست;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			بازگشت درست;
		حالت DriveType::CONSTANT:
			بازگشت درست;
		حالت DriveType::WIRE:
			بازگشت wire_.can_append(bit.wire());
		حالت DriveType::PORT:
			بازگشت port_.can_append(bit.port());
		حالت DriveType::MULTIPLE:
			بازگشت multiple_.can_append(bit.multiple());
		پیشفرض:
			log_abort();
	}
}

منطقی DriveChunk::try_append(DriveBit ثابت &bit)
{
	اگر (size() == 0)
		*این = bit;
	اگر (bit.type() != type_)
		بازگشت نادرست;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			none_ += 1;
			بازگشت درست;
		حالت DriveType::CONSTANT:
			constant_.append(RTLIL::Const(bit.constant()));
			بازگشت درست;
		حالت DriveType::WIRE:
			بازگشت wire_.try_append(bit.wire());
		حالت DriveType::PORT:
			بازگشت port_.try_append(bit.port());
		حالت DriveType::MULTIPLE:
			بازگشت multiple_.try_append(bit.multiple());
		پیشفرض:
			log_abort();
	}
}


منطقی DriveChunk::try_append(DriveChunk ثابت &chunk)
{
	اگر (size() == 0)
		*این = chunk;
	اگر (chunk.type_ != type_)
		بازگشت نادرست;
	انتخاب (type_)
	{
		حالت DriveType::NONE:
			none_ += chunk.none_;
			بازگشت درست;
		حالت DriveType::CONSTANT:
			constant_.append(chunk.constant_);
			بازگشت درست;
		حالت DriveType::WIRE:
			بازگشت wire_.try_append(chunk.wire());
		حالت DriveType::PORT:
			بازگشت port_.try_append(chunk.port());
		حالت DriveType::MARKER:
			بازگشت marker_.try_append(chunk.marker());
		حالت DriveType::MULTIPLE:
			بازگشت multiple_.try_append(chunk.multiple());
	}
	log_abort();
}

تهی DriveSpec::append(DriveBit ثابت &bit)
{
	hash_ = 0;
	اگر (!packed()) {
		bits_.افزودن_back(bit);
		width_ += 1;
		بازگشت;
	}

	اگر (chunks_.empty() || !chunks_.back().try_append(bit))
		chunks_.emplace_back(bit);
	width_ += 1;
}

تهی DriveSpec::append(DriveChunk ثابت &chunk)
{
	hash_ = 0;
	pack();
	اگر (chunks_.empty() || !chunks_.back().try_append(chunk))
		chunks_.emplace_back(chunk);
	width_ += chunk.size();
}

تهی DriveSpec::pack() ثابت {
	اگر (bits_.empty())
		بازگشت;
	std::vector<DriveBit> bits(std::move(bits_));
	برای (auto &bit : bits)
		اگر (chunks_.empty() || !chunks_.back().try_append(bit))
			chunks_.emplace_back(std::move(bit));
}

تهی DriveSpec::unpack() ثابت {
	اگر (chunks_.empty())
		بازگشت;
	برای (auto &chunk : chunks_)
	{
		برای (صحیح i = 0, width = chunk.size(); i != width; ++i)
		{
			bits_.emplace_back(chunk[i]);
		}
	}
	chunks_.clear();
}

تهی DriveSpec::compute_width()
{
	width_ = 0;
	برای (auto ثابت &chunk : chunks_)
		width_ += chunk.size();
}

تهی DriverMap::DriveBitGraph::add_edge(DriveBitId src, DriveBitId dst)
{
	اگر (first_edges.emplace(src, dst).first->second == dst)
		بازگشت;
	اگر (second_edges.emplace(src, dst).first->second == dst)
		بازگشت;
	more_edges[src].emplace(dst);
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::برداشت_edge(DriveBitId src)
{
	// TODO unused I think?
	auto found_more = more_edges.find(src);
	اگر (found_more != more_edges.end()) {
		auto result = found_more->second.برداشت();
		اگر (found_more->second.empty())
			more_edges.erase(found_more);
		بازگشت result;
	}

	auto found_second = second_edges.find(src);
	اگر (found_second != second_edges.end()) {
		auto result = found_second->second;
		second_edges.erase(found_second);
		بازگشت result;
	}

	auto found_first = first_edges.find(src);
	اگر (found_first != first_edges.end()) {
		auto result = found_first->second;
		first_edges.erase(found_first);
		بازگشت result;
	}

	بازگشت DriveBitId();
}

تهی DriverMap::DriveBitGraph::clear(DriveBitId src)
{
	first_edges.erase(src);
	second_edges.erase(src);
	more_edges.erase(src);
}

منطقی DriverMap::DriveBitGraph::contains(DriveBitId src)
{
	بازگشت first_edges.شمار(src);
}

صحیح DriverMap::DriveBitGraph::شمار(DriveBitId src)
{
	اگر (!first_edges.شمار(src))
		بازگشت 0;
	اگر (!second_edges.شمار(src))
		بازگشت 1;
	auto found = more_edges.find(src);
	اگر (found == more_edges.end())
		بازگشت 2;
	بازگشت GetSize(found->second) + 2;
}

DriverMap::DriveBitId DriverMap::DriveBitGraph::at(DriveBitId src, صحیح index)
{
	اگر (index == 0)
		بازگشت first_edges.at(src);
	وگرنه اگر (index == 1)
		بازگشت second_edges.at(src);
	وگرنه
		بازگشت *more_edges.at(src).element(index - 2);
}


DriverMap::BitMode DriverMap::bit_mode(DriveBit ثابت &bit)
{
	انتخاب (bit.type())
	{
		حالت DriveType::NONE:
			بازگشت BitMode::NONE;
		حالت DriveType::CONSTANT:
			// TODO how to handle Sx here?
			بازگشت bit.constant() == State::Sz ? BitMode::NONE : BitMode::DRIVER;
		حالت DriveType::WIRE: {
			auto ثابت &wire = bit.wire();
			منطقی driver = wire.wire->port_درون_داد;
			منطقی driven = wire.wire->port_output;

			اگر (driver && !driven)
				بازگشت BitMode::DRIVER;
			وگرنه اگر (driven && !driver)
				بازگشت BitMode::DRIVEN;
			وگرنه اگر (driver && driven)
				بازگشت BitMode::TRISTATE;
			وگرنه
				بازگشت keep_wire(bit.wire().wire) ? BitMode::KEEP : BitMode::NONE;
		}
		حالت DriveType::PORT: {
			auto ثابت &port = bit.port();
			منطقی driver = celltypes.cell_output(port.cell->type, port.port);
			منطقی driven = celltypes.cell_درون_داد(port.cell->type, port.port);
			اگر (driver && !driven)
				بازگشت BitMode::DRIVER;
			وگرنه اگر (driven && !driver)
				بازگشت BitMode::DRIVEN_UNIQUE;
			وگرنه
				بازگشت BitMode::TRISTATE;
		}
		حالت DriveType::MARKER: {
			// TODO user supplied classification
			log_abort();
		}
		پیشفرض:
			log_abort();
	}
}

DriverMap::DriveBitId DriverMap::id_from_drive_bit(DriveBit ثابت &bit)
{
	انتخاب (bit.type())
	{
		حالت DriveType::NONE:
			بازگشت -1;
		حالت DriveType::CONSTANT:
			بازگشت (صحیح)bit.constant();
		حالت DriveType::WIRE: {
			auto ثابت &wire_bit = bit.wire();
			صحیح offset = next_offset;
			auto insertion = wire_offsets.emplace(wire_bit.wire, offset);
			اگر (insertion.second) {
				اگر (wire_bit.wire->width == 1) {
					log_assert(wire_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} وگرنه
					drive_bits.emplace(offset, DriveBitWire(wire_bit.wire, 0));
				next_offset += wire_bit.wire->width;
			}
			بازگشت insertion.first->second.id + wire_bit.offset;
		}
		حالت DriveType::PORT: {
			auto ثابت &port_bit = bit.port();
			auto کلید = std::make_pair(port_bit.cell, port_bit.port);
			صحیح offset = next_offset;
			auto insertion = port_offsets.emplace(کلید, offset);
			اگر (insertion.second) {
				صحیح width = port_bit.cell->connections().at(port_bit.port).size();
				اگر (width == 1 && offset == 0) {
					log_assert(port_bit.offset == 0);
					isolated_drive_bits.emplace(offset, bit);
				} وگرنه
					drive_bits.emplace(offset, DriveBitPort(port_bit.cell, port_bit.port, 0));
				next_offset += width;
			}
			بازگشت insertion.first->second.id + port_bit.offset;
		}
		پیشفرض:
			log_assert(نادرست && "unsupported DriveType in DriverMap");
	}
	log_abort();
}

DriveBit DriverMap::drive_bit_from_id(DriveBitId id)
{
	auto found_isolated = isolated_drive_bits.find(id);
	اگر (found_isolated != isolated_drive_bits.end())
		بازگشت found_isolated->second;

	auto found = drive_bits.upper_bound(id);
	اگر (found == drive_bits.begin()) {
		بازگشت id < 0 ? DriveBit() : DriveBit((State) id.id);
	}
	--found;
	DriveBit result = found->second;
	اگر (result.is_wire()) {
		result.wire().offset += id.id - found->first.id;
	} وگرنه {
		log_assert(result.is_port());
		result.port().offset += id.id - found->first.id;
	}
	بازگشت result;
}

تهی DriverMap::connect_directed_merge(DriveBitId driven_id, DriveBitId driver_id)
{
	اگر (driven_id == driver_id)
		بازگشت;

	same_driver.merge(driven_id, driver_id);

	برای (صحیح i = 0, end = connected_drivers.شمار(driven_id); i != end; ++i)
		connected_drivers.add_edge(driver_id, connected_drivers.at(driven_id, i));

	connected_drivers.clear(driven_id);

	برای (صحیح i = 0, end = connected_undirected.شمار(driven_id); i != end; ++i)
		connected_undirected.add_edge(driver_id, connected_undirected.at(driven_id, i));

	connected_undirected.clear(driven_id);
}

تهی DriverMap::connect_directed_buffer(DriveBitId driven_id, DriveBitId driver_id)
{
	connected_drivers.add_edge(driven_id, driver_id);
}

تهی DriverMap::connect_undirected(DriveBitId a_id, DriveBitId b_id)
{
	connected_undirected.add_edge(a_id, b_id);
	connected_undirected.add_edge(b_id, a_id);
}

تهی DriverMap::add(Module *module)
{
	برای (auto ثابت &conn : module->connections())
		add(conn.first, conn.second);

	برای (auto cell : module->cells())
		برای (auto ثابت &conn : cell->connections())
			add_port(cell, conn.first, conn.second);
}

// Add a single bit connection to the driver map.
تهی DriverMap::add(DriveBit ثابت &a, DriveBit ثابت &b)
{
	DriveBitId a_id = id_from_drive_bit(a);
	DriveBitId b_id = id_from_drive_bit(b);

	DriveBitId orig_a_id = a_id;
	DriveBitId orig_b_id = b_id;

	a_id = same_driver.find(a_id);
	b_id = same_driver.find(b_id);

	اگر (a_id == b_id)
		بازگشت;

	BitMode a_mode = bit_mode(orig_a_id == a_id ? a : drive_bit_from_id(a_id));
	BitMode b_mode = bit_mode(orig_b_id == b_id ? b : drive_bit_from_id(b_id));

	// If either bit is just a wire that we don't need to keep, merge and
	// use the other end as representative bit.
	اگر (a_mode == BitMode::NONE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_merge(a_id, b_id);
	وگرنه اگر (b_mode == BitMode::NONE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_merge(b_id, a_id);
	// If either bit requires a driven value and has a unique driver, merge
	// and use the other end as representative bit.
	وگرنه اگر (a_mode == BitMode::DRIVEN_UNIQUE && !(b_mode == BitMode::DRIVEN_UNIQUE || b_mode == BitMode::DRIVEN))
		connect_directed_buffer(a_id, b_id);
	وگرنه اگر (b_mode == BitMode::DRIVEN_UNIQUE && !(a_mode == BitMode::DRIVEN_UNIQUE || a_mode == BitMode::DRIVEN))
		connect_directed_buffer(b_id, a_id);
	// If either bit only drives a value, store a directed connection from
	// it to the other bit.
	وگرنه اگر (a_mode == BitMode::DRIVER)
		connect_directed_buffer(b_id, a_id);
	وگرنه اگر (b_mode == BitMode::DRIVER)
		connect_directed_buffer(a_id, b_id);
	// Otherwise we store an undirected connection which we will resolve
	// during querying.
	وگرنه
		connect_undirected(a_id, b_id);

	بازگشت;
}

// Specialized version that avoids unpacking
تهی DriverMap::add(SigSpec ثابت &a, SigSpec ثابت &b)
{
	log_assert(a.size() == b.size());
	auto ثابت &a_chunks = a.chunks();
	auto ثابت &b_chunks = b.chunks();

	auto a_chunk = a_chunks.begin();
	auto a_end = a_chunks.end();
	صحیح a_offset = 0;

	auto b_chunk = b_chunks.begin();
	صحیح b_offset = 0;

	SigChunk tmp_a, tmp_b;
	وقتی (a_chunk != a_end) {
		صحیح a_width = a_chunk->width - a_offset;
		اگر (a_width == 0) {
			a_offset = 0;
			++a_chunk;
			ادامه;
		}
		صحیح b_width = b_chunk->width - b_offset;
		اگر (b_width == 0) {
			b_offset = 0;
			++b_chunk;
			ادامه;
		}
		صحیح width = std::min(a_width, b_width);
		log_assert(width > 0);

		SigChunk ثابت &a_subchunk =
			a_offset == 0 && a_width == width ? *a_chunk : a_chunk->extract(a_offset, width);
		SigChunk ثابت &b_subchunk =
			b_offset == 0 && b_width == width ? *b_chunk : b_chunk->extract(b_offset, width);

		add(a_subchunk, b_subchunk);

		a_offset += width;
		b_offset += width;
	}
}

تهی DriverMap::add_port(Cell *cell, IdString port, SigSpec ثابت &b)
{
	صحیح offset = 0;
	برای (auto ثابت &chunk : b.chunks()) {
		add(chunk, DriveChunkPort(cell, port, offset, chunk.width));
		offset += chunk.size();
	}
}

تهی DriverMap::orient_undirected(DriveBitId id)
{
	pool<DriveBitId> &seen = orient_undirected_seen;
	pool<DriveBitId> &drivers = orient_undirected_drivers;
	dict<DriveBitId, صحیح> &distance = orient_undirected_distance;
	seen.clear();
	drivers.clear();

	seen.emplace(id);

	برای (صحیح pos = 0; pos < GetSize(seen); ++pos) {
		DriveBitId current = *seen.element(seen.size() - 1 - pos);
		DriveBit bit = drive_bit_from_id(current);

		BitMode mode = bit_mode(bit);

		اگر (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
			drivers.emplace(current);

		اگر (connected_drivers.contains(current))
			drivers.emplace(current);

		صحیح undirected_driver_شمار = connected_undirected.شمار(current);

		برای (صحیح i = 0; i != undirected_driver_شمار; ++i)
			seen.emplace(same_driver.find(connected_undirected.at(current, i)));
	}

	اگر (drivers.empty())
		برای (auto seen_id : seen)
			drivers.emplace(seen_id);

	برای (auto driver : drivers)
	{
		distance.clear();
		distance.emplace(driver, 0);

		برای (صحیح pos = 0; pos < GetSize(distance); ++pos) {
			auto current_it = distance.element(distance.size() - 1 - pos);

			DriveBitId current = current_it->first;
			صحیح undirected_driver_شمار = connected_undirected.شمار(current);

			برای (صحیح i = 0; i != undirected_driver_شمار; ++i)
			{
				DriveBitId next = same_driver.find(connected_undirected.at(current, i));
				auto emplaced = distance.emplace(next, current_it->second + 1);
				اگر (emplaced.first->second == current_it->second + 1)
					connected_oriented.add_edge(next, current);
			}
		}
	}

	برای (auto seen_id : seen)
		oriented_present.emplace(seen_id);
}

DriveBit DriverMap::operator()(DriveBit ثابت &bit)
{
	اگر (bit.type() == DriveType::MARKER || bit.type() == DriveType::NONE)
		بازگشت bit;
	اگر (bit.type() == DriveType::MULTIPLE)
	{
		DriveBit result;
		برای (auto ثابت &inner : bit.multiple().multiple())
			result.merge((*این)(inner));
		بازگشت result;
	}

	DriveBitId bit_id = id_from_drive_bit(bit);

	DriveBitId bit_repr_id = same_driver.find(bit_id);

	DriveBit bit_repr = drive_bit_from_id(bit_repr_id);

	BitMode mode = bit_mode(bit_repr);

	اگر (mode == BitMode::KEEP && bit_repr_id != bit_id)
		بازگشت bit_repr;

	صحیح implicit_driver_شمار = connected_drivers.شمار(bit_repr_id);
	اگر (connected_undirected.contains(bit_repr_id) && !oriented_present.شمار(bit_repr_id))
		orient_undirected(bit_repr_id);

	DriveBit driver;

	اگر (mode == BitMode::DRIVER || mode == BitMode::TRISTATE)
		driver = bit_repr;

	برای (صحیح i = 0; i != implicit_driver_شمار; ++i)
		driver.merge(drive_bit_from_id(connected_drivers.at(bit_repr_id, i)));

	صحیح oriented_driver_شمار = connected_oriented.شمار(bit_repr_id);
	برای (صحیح i = 0; i != oriented_driver_شمار; ++i)
		driver.merge(drive_bit_from_id(connected_oriented.at(bit_repr_id, i)));

	بازگشت driver;
}

DriveSpec DriverMap::operator()(DriveSpec spec)
{
	DriveSpec result;

	برای (صحیح i = 0, width = spec.size(); i != width; ++i)
		result.append((*این)(spec[i]));

	بازگشت result;
}

std::string log_signal(DriveChunkWire ثابت &chunk)
{
	std::string id = chunk.wire->name.unescape();
	اگر (chunk.is_whole())
		بازگشت id;
	اگر (chunk.width == 1)
		بازگشت stringf("%s [%d]", id, chunk.offset);
	بازگشت stringf("%s [%d:%d]", id, chunk.offset + chunk.width - 1, chunk.offset);
}


std::string log_signal(DriveChunkPort ثابت &chunk)
{
	std::string cell_id = chunk.cell->name.unescape();
	std::string port_id = chunk.cell->twines().unescaped_str(chunk.port);
	اگر (chunk.is_whole())
		بازگشت stringf("%s <%s>", cell_id, port_id);
	اگر (chunk.width == 1)
		بازگشت stringf("%s <%s> [%d]", cell_id, port_id, chunk.offset);
	بازگشت stringf("%s <%s> [%d:%d]", cell_id, port_id, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunkMarker ثابت &chunk)
{
	اگر (chunk.width == 1)
		بازگشت stringf("<marker %d> [%d]", chunk.marker, chunk.offset);
	بازگشت stringf("<marker %d> [%d:%d]", chunk.marker, chunk.offset + chunk.width - 1, chunk.offset);
}

std::string log_signal(DriveChunk ثابت &chunk)
{
	انتخاب (chunk.type())
	{
		حالت DriveType::NONE:
			بازگشت stringf("<none x%d>", chunk.size());
		حالت DriveType::CONSTANT:
			بازگشت log_const(chunk.constant());
		حالت DriveType::WIRE:
			بازگشت log_signal(chunk.wire());
		حالت DriveType::PORT:
			بازگشت log_signal(chunk.port());
		حالت DriveType::MARKER:
			بازگشت log_signal(chunk.marker());
		حالت DriveType::MULTIPLE: {
			std::string str = "<multiple";
			ثابت نویسه *sep = " ";
			برای (auto ثابت &single : chunk.multiple().multiple()) {
				str += sep;
				sep = ", ";
				str += log_signal(single);
			}
			str += ">";
			بازگشت str;
		}
		پیشفرض:
			log_abort();
	}
}

std::string log_signal(DriveSpec ثابت &spec)
{
	auto &chunks = spec.chunks();
	اگر (chunks.empty())
		بازگشت "{}";
	اگر (chunks.size() == 1)
		بازگشت log_signal(chunks[0]);

	std::string str;
	ثابت نویسه *sep = "{ ";

	برای (auto i = chunks.rbegin(), end = chunks.rend(); i != end; ++i)
	{
		str += sep;
		sep = " ";
		str += log_signal(*i);
	}
	str += " }";

	بازگشت str;
}

YOSYS_NAMESPACE_END
