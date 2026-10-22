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

#अगर_अघोषित MEM_H
#घोषणा MEM_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/ffinit.h"
#समावेश "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	पूर्णांक wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_मूल्य, srst_मूल्य, init_मूल्य;
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
	SigSpec clk, en, arst, srst, addr, तथ्य;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(पूर्णांक sub) {
		SigSpec res = addr;
		क्रम (पूर्णांक i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		वापस res;
	}
};

काष्ठा MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	पूर्णांक wide_log2;
	bool clk_enable, clk_polarity;
	std::vector<bool> priority_mask;
	SigSpec clk, en, addr, तथ्य;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(पूर्णांक sub) {
		SigSpec res = addr;
		क्रम (पूर्णांक i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		वापस res;
	}

	std::pair<SigSpec, std::vector<पूर्णांक>> compress_en();
	SigSpec decompress_en(स्थिर std::vector<पूर्णांक> &swizzle, SigSpec sig);
};

काष्ठा MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const तथ्य;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

काष्ठा Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::Memory *mem;
	Cell *cell;
	पूर्णांक width, start_offset, आकार;
	std::vector<MemInit> inits;
	std::vector<MemRd> rd_ports;
	std::vector<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	व्योम हटाओ();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	व्योम emit();

	// Marks all inits as removed.
	व्योम clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	व्योम coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	व्योम check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_तथ्य() स्थिर;

	// Constructs and returns the helper structures for all memories
	// in a module.
	अटल std::vector<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	अटल std::vector<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *extract_rdff(पूर्णांक idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	व्योम narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	व्योम emulate_priority(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	व्योम emulate_transparency(पूर्णांक widx, पूर्णांक ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	व्योम prepare_wr_merge(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	व्योम prepare_rd_merge(पूर्णांक idx1, पूर्णांक idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	व्योम widen_prep(पूर्णांक wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	व्योम widen_wr_port(पूर्णांक idx, पूर्णांक wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	व्योम emulate_rden(पूर्णांक idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	व्योम emulate_reset(पूर्णांक idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	व्योम emulate_rd_ce_over_srst(पूर्णांक idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	व्योम emulate_rd_srst_over_ce(पूर्णांक idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_first_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	व्योम emulate_read_first(FfInitVals *initvals);

	Mem(Module *module, IdString memid, पूर्णांक width, पूर्णांक start_offset, पूर्णांक आकार) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), width(width), start_offset(start_offset), आकार(आकार) {}
	Mem(Module *module, TwineSpec &&memid, पूर्णांक width, पूर्णांक start_offset, पूर्णांक आकार)
			: Mem(module, module->twines().add(std::move(memid)), width, start_offset, आकार) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
खुला:
	class range; class iterator;
	using addr_t = uint32_t;
निजी:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	पूर्णांक _addr_width;
	पूर्णांक तथ्य_width_2;
	RTLIL::Const default_मूल्य;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> मूल्यावली_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) स्थिर;
	addr_t range_आकार(std::map<addr_t, RTLIL::Const>::iterator it) स्थिर { वापस it->second.आकार() / तथ्य_width_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) स्थिर { वापस it->first; }
	addr_t _range_end(std::map<addr_t, RTLIL::Const>::iterator it) स्थिर { वापस _range_begin(it) + range_आकार(it); }
	// check if the iterator points to a range containing addr
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) स्थिर;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool _range_contains(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) स्थिर;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t end_addr) स्थिर;
	// return the offset the addr would have in the range at `it`
	माप_प्रकार _range_offset(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) स्थिर { वापस (addr - it->first) * तथ्य_width_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_तथ्य(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		वापस RTLIL::Const::iterator(it->second, _range_offset(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t end_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const स्थिर &तथ्य);
खुला:
	class range {
		पूर्णांक तथ्य_width_2;
		addr_t _base;
		RTLIL::Const स्थिर &मूल्यावली_2;
		मित्र class iterator;
		range(पूर्णांक तथ्य_width, addr_t base, RTLIL::Const स्थिर &मूल्यावली)
		: तथ्य_width_2(तथ्य_width), _base(base), मूल्यावली_2(मूल्यावली) {}
	खुला:
		addr_t base() स्थिर { वापस _base; }
		addr_t आकार() स्थिर { वापस ((addr_t) मूल्यावली_2.आकार()) / तथ्य_width_2; }
		addr_t limit() स्थिर { वापस _base + आकार(); }
		RTLIL::Const स्थिर &concatenated() स्थिर { वापस मूल्यावली_2; }
		RTLIL::Const चालक[](addr_t addr) स्थिर {
			log_assert(addr - _base < आकार());
			वापस मूल्यावली_2.extract((addr - _base) * तथ्य_width_2, तथ्य_width_2);
		}
		RTLIL::Const at_offset(addr_t offset) स्थिर { वापस (*this)[_base + offset]; }
	};
	class iterator {
		MemContents स्थिर *_memory;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		मित्र class MemContents;
		iterator(MemContents स्थिर *memory, addr_t addr) : _memory(memory), _addr(addr) {}
	खुला:
		using iterator_category = std::निवेश_iterator_tag;
		using मूल्य_type = range;
		using pointer = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference चालक *() स्थिर { वापस range(_memory->तथ्य_width_2, _addr, _memory->मूल्यावली_2.at(_addr)); }
		pointer चालक->() स्थिर { वापस arrow_proxy<range>(**this); }
		bool चालक !=(iterator स्थिर &other) स्थिर { वापस _memory != other._memory || _addr != other._addr; }
		bool चालक ==(iterator स्थिर &other) स्थिर { वापस !(*this != other); }
		iterator &चालक++();
	};
	MemContents(पूर्णांक addr_width, पूर्णांक तथ्य_width, RTLIL::Const default_मूल्य_2)
		: _addr_width(addr_width), तथ्य_width_2(तथ्य_width)
		, default_मूल्य((default_मूल्य_2.extu(तथ्य_width), std::move(default_मूल्य_2)))
	{ log_assert(_addr_width > 0 && _addr_width < (पूर्णांक)माप(addr_t) * 8); log_assert(तथ्य_width_2 > 0); }
	MemContents(पूर्णांक addr_width, पूर्णांक तथ्य_width) : MemContents(addr_width, तथ्य_width, RTLIL::Const(State::Sx, तथ्य_width)) {}
	explicit MemContents(Mem *mem);
	पूर्णांक addr_width() स्थिर { वापस _addr_width; }
	पूर्णांक तथ्य_width() स्थिर { वापस तथ्य_width_2; }
	RTLIL::Const स्थिर &default_मूल्य_2() स्थिर { वापस default_मूल्य; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const चालक [](addr_t addr) स्थिर;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t गिनती_range(addr_t begin_addr, addr_t end_addr) स्थिर;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	व्योम reserve_range(addr_t begin_addr, addr_t end_addr) { _reserve_range(begin_addr, end_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	व्योम जोड़ो_concatenated(addr_t addr, RTLIL::Const स्थिर &मूल्यावली);
	// insert multiple words at the given address, overriding any previous assignment.
	ढाँचा<typename Iterator> व्योम जोड़ो_range(addr_t addr, Iterator begin, Iterator end) {
		स्वतः शब्दगण_2 = end - begin;
		log_assert(addr < (addr_t)(1<<_addr_width)); log_assert(शब्दगण_2 <= (addr_t)(1<<_addr_width) - addr);
		स्वतः range = _reserve_range(addr, addr + शब्दगण_2);
		स्वतः it = range_तथ्य(range, addr);
		क्रम(; begin != end; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	व्योम clear_range(addr_t begin_addr, addr_t end_addr);
	// check invariants, abort if invariants failed
	व्योम check();
	iterator end() स्थिर { वापस iterator(nullptr, ~(addr_t) 0); }
	iterator begin() स्थिर { वापस मूल्यावली_2.empty() ? end() : iterator(this, मूल्यावली_2.begin()->first); }
	bool empty() स्थिर { वापस मूल्यावली_2.empty(); }
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
