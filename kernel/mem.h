/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
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

#ifndef MEM_H
#define MEM_H

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/ffinit.h"
#അന്തര്ഭാവ "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

ഘടന MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	പൂർണ്ണസംഖ്യ wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_മൂല്യം, srst_മൂല്യം, init_മൂല്യം;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::vector<bool> transparency_mask;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::vector<bool> collision_x_mask;
	SigSpec clk, en, arst, srst, addr, data;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(പൂർണ്ണസംഖ്യ sub) {
		SigSpec res = addr;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		തിരികെ res;
	}
};

ഘടന MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	പൂർണ്ണസംഖ്യ wide_log2;
	bool clk_enable, clk_polarity;
	std::vector<bool> priority_mask;
	SigSpec clk, en, addr, data;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(പൂർണ്ണസംഖ്യ sub) {
		SigSpec res = addr;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		തിരികെ res;
	}

	std::pair<SigSpec, std::vector<പൂർണ്ണസംഖ്യ>> compress_en();
	SigSpec decompress_en(സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &swizzle, SigSpec sig);
};

ഘടന MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const data;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

ഘടന Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::Memory *mem;
	Cell *cell;
	പൂർണ്ണസംഖ്യ width, start_offset, size;
	std::vector<MemInit> inits;
	std::vector<MemRd> rd_ports;
	std::vector<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	ശൂന്യം remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	ശൂന്യം emit();

	// Marks all inits as removed.
	ശൂന്യം clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	ശൂന്യം coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	ശൂന്യം check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_data() സ്ഥിരം;

	// Constructs and returns the helper structures for all memories
	// in a module.
	സ്ഥിതം std::vector<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	സ്ഥിതം std::vector<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *extract_rdff(പൂർണ്ണസംഖ്യ idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	ശൂന്യം narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	ശൂന്യം emulate_priority(പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	ശൂന്യം emulate_transparency(പൂർണ്ണസംഖ്യ widx, പൂർണ്ണസംഖ്യ ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	ശൂന്യം prepare_wr_merge(പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	ശൂന്യം prepare_rd_merge(പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	ശൂന്യം widen_prep(പൂർണ്ണസംഖ്യ wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	ശൂന്യം widen_wr_port(പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	ശൂന്യം emulate_rden(പൂർണ്ണസംഖ്യ idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	ശൂന്യം emulate_reset(പൂർണ്ണസംഖ്യ idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	ശൂന്യം emulate_rd_ce_over_srst(പൂർണ്ണസംഖ്യ idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	ശൂന്യം emulate_rd_srst_over_ce(പൂർണ്ണസംഖ്യ idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_first_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	ശൂന്യം emulate_read_first(FfInitVals *initvals);

	Mem(Module *module, IdString memid, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ start_offset, പൂർണ്ണസംഖ്യ size) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), width(width), start_offset(start_offset), size(size) {}
	Mem(Module *module, TwineSpec &&memid, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ start_offset, പൂർണ്ണസംഖ്യ size)
			: Mem(module, module->twines().add(std::move(memid)), width, start_offset, size) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
പൊതു:
	class range; class iterator;
	using addr_t = uint32_t;
സ്വകാര്യ:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	പൂർണ്ണസംഖ്യ _addr_width;
	പൂർണ്ണസംഖ്യ _data_width;
	RTLIL::Const default_മൂല്യം;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> മൂല്യങ്ങൾ_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) സ്ഥിരം;
	addr_t _range_size(std::map<addr_t, RTLIL::Const>::iterator it) സ്ഥിരം { തിരികെ it->second.size() / _data_width; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) സ്ഥിരം { തിരികെ it->first; }
	addr_t _range_end(std::map<addr_t, RTLIL::Const>::iterator it) സ്ഥിരം { തിരികെ _range_begin(it) + _range_size(it); }
	// check if the iterator points to a range containing addr
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) സ്ഥിരം;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) സ്ഥിരം;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) സ്ഥിരം;
	// return the offset the addr would have in the range at `it`
	size_t _range_offset(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) സ്ഥിരം { തിരികെ (addr - it->first) * _data_width; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator _range_data(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		തിരികെ RTLIL::Const::iterator(it->second, _range_offset(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t end_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const സ്ഥിരം &data);
പൊതു:
	class range {
		പൂർണ്ണസംഖ്യ _data_width;
		addr_t _base;
		RTLIL::Const സ്ഥിരം &മൂല്യങ്ങൾ_2;
		friend class iterator;
		range(പൂർണ്ണസംഖ്യ data_width, addr_t base, RTLIL::Const സ്ഥിരം &മൂല്യങ്ങൾ)
		: _data_width(data_width), _base(base), മൂല്യങ്ങൾ_2(മൂല്യങ്ങൾ) {}
	പൊതു:
		addr_t base() സ്ഥിരം { തിരികെ _base; }
		addr_t size() സ്ഥിരം { തിരികെ ((addr_t) മൂല്യങ്ങൾ_2.size()) / _data_width; }
		addr_t limit() സ്ഥിരം { തിരികെ _base + size(); }
		RTLIL::Const സ്ഥിരം &concatenated() സ്ഥിരം { തിരികെ മൂല്യങ്ങൾ_2; }
		RTLIL::Const operator[](addr_t addr) സ്ഥിരം {
			log_assert(addr - _base < size());
			തിരികെ മൂല്യങ്ങൾ_2.extract((addr - _base) * _data_width, _data_width);
		}
		RTLIL::Const at_offset(addr_t offset) സ്ഥിരം { തിരികെ (*this)[_base + offset]; }
	};
	class iterator {
		MemContents സ്ഥിരം *_memory;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		friend class MemContents;
		iterator(MemContents സ്ഥിരം *memory, addr_t addr) : _memory(memory), _addr(addr) {}
	പൊതു:
		using iterator_category = std::നിവേശം_iterator_tag;
		using മൂല്യം_type = range;
		using pointer = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference operator *() സ്ഥിരം { തിരികെ range(_memory->_data_width, _addr, _memory->മൂല്യങ്ങൾ_2.at(_addr)); }
		pointer operator->() സ്ഥിരം { തിരികെ arrow_proxy<range>(**this); }
		bool operator !=(iterator സ്ഥിരം &other) സ്ഥിരം { തിരികെ _memory != other._memory || _addr != other._addr; }
		bool operator ==(iterator സ്ഥിരം &other) സ്ഥിരം { തിരികെ !(*this != other); }
		iterator &operator++();
	};
	MemContents(പൂർണ്ണസംഖ്യ addr_width, പൂർണ്ണസംഖ്യ data_width, RTLIL::Const default_മൂല്യം_2)
		: _addr_width(addr_width), _data_width(data_width)
		, default_മൂല്യം((default_മൂല്യം_2.extu(data_width), std::move(default_മൂല്യം_2)))
	{ log_assert(_addr_width > 0 && _addr_width < (പൂർണ്ണസംഖ്യ)വലുപ്പം(addr_t) * 8); log_assert(_data_width > 0); }
	MemContents(പൂർണ്ണസംഖ്യ addr_width, പൂർണ്ണസംഖ്യ data_width) : MemContents(addr_width, data_width, RTLIL::Const(State::Sx, data_width)) {}
	explicit MemContents(Mem *mem);
	പൂർണ്ണസംഖ്യ addr_width() സ്ഥിരം { തിരികെ _addr_width; }
	പൂർണ്ണസംഖ്യ data_width() സ്ഥിരം { തിരികെ _data_width; }
	RTLIL::Const സ്ഥിരം &default_മൂല്യം_2() സ്ഥിരം { തിരികെ default_മൂല്യം; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const operator [](addr_t addr) സ്ഥിരം;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t എണ്ണം_range(addr_t begin_addr, addr_t end_addr) സ്ഥിരം;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	ശൂന്യം reserve_range(addr_t begin_addr, addr_t end_addr) { _reserve_range(begin_addr, end_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	ശൂന്യം തിരുകുക_concatenated(addr_t addr, RTLIL::Const സ്ഥിരം &മൂല്യങ്ങൾ);
	// insert multiple words at the given address, overriding any previous assignment.
	template<typename Iterator> ശൂന്യം തിരുകുക_range(addr_t addr, Iterator begin, Iterator end) {
		auto വാക്കുകൾ_2 = end - begin;
		log_assert(addr < (addr_t)(1<<_addr_width)); log_assert(വാക്കുകൾ_2 <= (addr_t)(1<<_addr_width) - addr);
		auto range = _reserve_range(addr, addr + വാക്കുകൾ_2);
		auto it = _range_data(range, addr);
		വേണ്ടി(; begin != end; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	ശൂന്യം clear_range(addr_t begin_addr, addr_t end_addr);
	// check invariants, abort if invariants failed
	ശൂന്യം check();
	iterator end() സ്ഥിരം { തിരികെ iterator(nullptr, ~(addr_t) 0); }
	iterator begin() സ്ഥിരം { തിരികെ മൂല്യങ്ങൾ_2.empty() ? end() : iterator(this, മൂല്യങ്ങൾ_2.begin()->first); }
	bool empty() സ്ഥിരം { തിരികെ മൂല്യങ്ങൾ_2.empty(); }
};

YOSYS_NAMESPACE_END

#endif
