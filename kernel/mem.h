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

#यदि_अपरिभाषितम् MEM_H
#परिभाषय MEM_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/ffinit.h"
#अन्तर्भाव "kernel/utils.h"

YOSYS_NAMESPACE_BEGIN

संरचना MemRd : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	पूर्णाङ्क wide_log2;
	bool clk_enable, clk_polarity, ce_over_srst;
	Const arst_मानम्, srst_मानम्, init_मानम्;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will bypass the written data
	// to this port's output (default behavior is to read old value).
	// Can only be set for write ports that have the same clock domain.
	std::सदिशम्<bool> transparency_आवरणम्;
	// One bit for every write port, true iff simultanous read on this
	// port and write on the other port will return an all-X (don't care)
	// value.  Mutually exclusive with transparency_mask.
	// Can only be set for write ports that have the same clock domain.
	// For optimization purposes, this will also be set if we can
	// determine that the two ports can never be active simultanously
	// (making the above vacuously true).
	std::सदिशम्<bool> collision_x_आवरणम्;
	SigSpec clk, en, arst, srst, addr, दत्त;

	MemRd() : removed(false), cell(nullptr), wide_log2(0), clk_enable(false), clk_polarity(true), ce_over_srst(false), clk(State::Sx), en(State::S1), arst(State::S0), srst(State::S0) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(पूर्णाङ्क sub) {
		SigSpec res = addr;
		प्रति (पूर्णाङ्क i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		प्रत्यागम res;
	}
};

संरचना MemWr : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	पूर्णाङ्क wide_log2;
	bool clk_enable, clk_polarity;
	std::सदिशम्<bool> प्राथमिकता_आवरणम्;
	SigSpec clk, en, addr, दत्त;

	MemWr() : removed(false), cell(nullptr) {}

	// Returns the address of given subword index accessed by this port.
	SigSpec sub_addr(पूर्णाङ्क sub) {
		SigSpec res = addr;
		प्रति (पूर्णाङ्क i = 0; i < wide_log2; i++)
			res[i] = State(sub >> i & 1);
		प्रत्यागम res;
	}

	std::pair<SigSpec, std::सदिशम्<पूर्णाङ्क>> compress_en();
	SigSpec decompress_en(ध्रुव std::सदिशम्<पूर्णाङ्क> &swizzle, SigSpec sig);
};

संरचना MemInit : RTLIL::AttrObject {
	bool removed;
	Cell *cell;
	Const addr;
	Const दत्त;
	Const en;
	MemInit() : removed(false), cell(nullptr) {}
};

संरचना Mem : RTLIL::AttrObject {
	Module *module;
	PooledName memid;
	bool packed;
	RTLIL::स्मृतिः *mem;
	Cell *cell;
	पूर्णाङ्क विस्तारः, start_विस्थापनम्, आकार;
	std::सदिशम्<MemInit> inits;
	std::सदिशम्<MemRd> rd_ports;
	std::सदिशम्<MemWr> wr_ports;

	// Removes this memory from the module.  The data in helper structures
	// is unaffected except for the cell/mem fields.
	शून्य remove();

	// Commits all changes in helper structures into the module — ports and
	// inits marked as removed are actually removed, new ports/inits create
	// new cells, modified port/inits are commited into their existing
	// cells.  Note that this reindexes the ports and inits array (actually
	// removing the ports/inits marked as removed).
	शून्य emit();

	// Marks all inits as removed.
	शून्य clear_inits();

	// Coalesces inits: whenever two inits have overlapping or touching
	// address ranges, they are combined into one, with the higher-priority
	// one's data overwriting the other.  Running this results in
	// an inits list equivalent to the original, in which all entries
	// cover disjoint (and non-touching) address ranges, and all enable
	// masks are all-1.
	शून्य coalesce_inits();

	// Checks consistency of this memory and all its ports/inits, using
	// log_assert.
	शून्य check();

	// Gathers all initialization data into a single big const covering
	// the whole memory.  For all non-initialized bits, Sx will be returned.
	Const get_init_दत्त() ध्रुव;

	// Constructs and returns the helper structures for all memories
	// in a module.
	स्थिर std::सदिशम्<Mem> get_all_memories(Module *module);

	// Constructs and returns the helper structures for all selected
	// memories in a module.
	स्थिर std::सदिशम्<Mem> get_selected_memories(Module *module);

	// Converts a synchronous read port into an asynchronous one by
	// extracting the data (or, in some rare cases, address) register
	// into a separate cell, together with any soft-transparency
	// logic necessary to preserve its semantics.  Returns the created
	// register cell, if any.  Note that in some rare cases this function
	// may succeed and perform a conversion without creating a new
	// register — a nullptr result doesn't imply nothing was done.
	Cell *extract_rdff(पूर्णाङ्क idx, FfInitVals *initvals);

	// Splits all wide ports in this memory into equivalent narrow ones.
	// This function performs no modifications at all to the actual
	// netlist unless and until emit() is called.
	शून्य narrow();

	// If write port idx2 currently has priority over write port idx1,
	// inserts extra logic on idx1's enable signal to disable writes
	// when idx2 is writing to the same address, then removes the priority
	// from the priority mask.  If there is a memory port that is
	// transparent with idx1, but not with idx2, that port is converted
	// to use soft transparency logic.
	शून्य emulate_प्राथमिकता(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals);

	// Creates soft-transparency logic on read port ridx, bypassing the
	// data from write port widx.  Should only be called when ridx is
	// transparent wrt widx in the first place.  Once we're done, the
	// transparency_mask bit will be cleared, and the collision_x_mask
	// bit will be set instead (since whatever value is read will be
	// replaced by the soft transparency logic).
	शून्य emulate_transparency(पूर्णाङ्क widx, पूर्णाङ्क ridx, FfInitVals *initvals);

	// Prepares for merging write port idx2 into idx1 (where idx1 < idx2).
	// Specifically, takes care of priority masks: any priority relations
	// that idx2 had are replicated onto idx1, unless they conflict with
	// priorities already present on idx1, in which case emulate_priority
	// is called.  Likewise, ensures transparency and undefined collision
	// masks of all read ports have the same values for both ports,
	// calling emulate_transparency if necessary.
	शून्य prepare_wr_merge(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals);

	// Prepares for merging read port idx2 into idx1.
	// Specifically, makes sure the transparency and undefined collision
	// masks of both ports are equal, by changing undefined behavior
	// of one port to the other's defined behavior, or by calling
	// emulate_transparency if necessary.
	शून्य prepare_rd_merge(पूर्णाङ्क idx1, पूर्णाङ्क idx2, FfInitVals *initvals);

	// Prepares the memory for widening a port to a given width.  This
	// involves ensuring that start_offset and size are aligned to the
	// target width.
	शून्य widen_prep(पूर्णाङ्क wide_log2);

	// Widens a write port up to a given width.  The newly port is
	// equivalent to the original, made by replicating enable/data bits
	// and masking enable bits with decoders on the low part of the
	// original address.
	शून्य widen_wr_पत्तनम्(पूर्णाङ्क idx, पूर्णाङ्क wide_log2);

	// Emulates a sync read port's enable functionality in soft logic,
	// changing the actual read port's enable to be always-on.
	शून्य emulate_rden(पूर्णाङ्क idx, FfInitVals *initvals);

	// Emulates a sync read port's initial/reset value functionality in
	// soft logic, removing it from the actual read port.
	शून्य emulate_reset(पूर्णाङ्क idx, bool emu_init, bool emu_arst, bool emu_srst, FfInitVals *initvals);

	// Given a read port with ce_over_srst set, converts it to a port
	// with ce_over_srst unset without changing its behavior by adding
	// emulation logic.
	शून्य emulate_rd_ce_over_srst(पूर्णाङ्क idx);

	// Given a read port with ce_over_srst unset, converts it to a port
	// with ce_over_srst set without changing its behavior by adding
	// emulation logic.
	शून्य emulate_rd_srst_over_ce(पूर्णाङ्क idx);

	// Returns true iff emulate_read_first makes sense to call.
	bool emulate_read_first_ok();

	// Emulates all read-first read-write port relationships in terms of
	// all-transparent ports, by delaying all write ports by one cycle.
	// This can only be used when all read ports and all write ports are
	// in the same clock domain.
	शून्य emulate_read_first(FfInitVals *initvals);

	Mem(Module *module, IdString memid, पूर्णाङ्क विस्तारः, पूर्णाङ्क start_विस्थापनम्, पूर्णाङ्क आकार) : module(module), memid(module->design, memid), packed(false), mem(nullptr), cell(nullptr), विस्तारः(विस्तारः), start_विस्थापनम्(start_विस्थापनम्), आकार(आकार) {}
	Mem(Module *module, TwineSpec &&memid, पूर्णाङ्क विस्तारः, पूर्णाङ्क start_विस्थापनम्, पूर्णाङ्क आकार)
			: Mem(module, module->twines().सङ्कलय(std::move(memid)), विस्तारः, start_विस्थापनम्, आकार) {}
};

// MemContents efficiently represents the contents of a potentially sparse memory by storing only those segments that are actually defined
class MemContents {
सार्वजनिक:
	class range; class iterator;
	using addr_t = uint32_t;
गुप्त:
	// we ban _addr_width == sizeof(addr_t) * 8 because it adds too many cornercases
	पूर्णाङ्क addr_विस्तारः_2;
	पूर्णाङ्क दत्त_विस्तारः_2;
	RTLIL::Const default_मानम्;
	// for each range, store the concatenation of the words at the start address
	// invariants:
	// - no overlapping or adjacent ranges
	// - no empty ranges
	// - all Consts are a multiple of the word size
	std::map<addr_t, RTLIL::Const> मानानि_2;
	// returns an iterator to the range containing addr, if it exists, or the first range past addr
	std::map<addr_t, RTLIL::Const>::iterator _range_at(addr_t addr) ध्रुव;
	addr_t range_आकार(std::map<addr_t, RTLIL::Const>::iterator it) ध्रुव { प्रत्यागम it->second.आकार() / दत्त_विस्तारः_2; }
	addr_t _range_begin(std::map<addr_t, RTLIL::Const>::iterator it) ध्रुव { प्रत्यागम it->first; }
	addr_t range_अन्तः(std::map<addr_t, RTLIL::Const>::iterator it) ध्रुव { प्रत्यागम _range_begin(it) + range_आकार(it); }
	// check if the iterator points to a range containing addr
	bool range_अन्तर्भवति(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ध्रुव;
	// check if the iterator points to a range containing [begin_addr, end_addr). assumes end_addr >= begin_addr.
	bool range_अन्तर्भवति(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t अन्तः_addr) ध्रुव;
	// check if the iterator points to a range overlapping with [begin_addr, end_addr)
	bool _range_overlaps(std::map<addr_t, RTLIL::Const>::iterator it, addr_t begin_addr, addr_t अन्तः_addr) ध्रुव;
	// return the offset the addr would have in the range at `it`
	आकार_t range_विस्थापनम्(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) ध्रुव { प्रत्यागम (addr - it->first) * दत्त_विस्तारः_2; }
	// assuming _range_contains(it, addr), return an iterator pointing to the data at addr
	RTLIL::Const::iterator range_दत्त(std::map<addr_t, RTLIL::Const>::iterator it, addr_t addr) {
		प्रत्यागम RTLIL::Const::iterator(it->second, range_विस्थापनम्(it, addr));
	}
	// internal version of reserve_range that returns an iterator to the range
	std::map<addr_t, RTLIL::Const>::iterator _reserve_range(addr_t begin_addr, addr_t अन्तः_addr);
	// write a single word at addr, return iterator to next word
	RTLIL::Const::iterator _range_write(RTLIL::Const::iterator it, RTLIL::Const ध्रुव &दत्त);
सार्वजनिक:
	class range {
		पूर्णाङ्क दत्त_विस्तारः_2;
		addr_t आधारः_2;
		RTLIL::Const ध्रुव &मानानि_2;
		मित्रम् class iterator;
		range(पूर्णाङ्क दत्त_विस्तारः, addr_t आधारः, RTLIL::Const ध्रुव &मानानि)
		: दत्त_विस्तारः_2(दत्त_विस्तारः), आधारः_2(आधारः), मानानि_2(मानानि) {}
	सार्वजनिक:
		addr_t आधारः() ध्रुव { प्रत्यागम आधारः_2; }
		addr_t आकार() ध्रुव { प्रत्यागम ((addr_t) मानानि_2.आकार()) / दत्त_विस्तारः_2; }
		addr_t सीमा() ध्रुव { प्रत्यागम आधारः_2 + आकार(); }
		RTLIL::Const ध्रुव &concatenated() ध्रुव { प्रत्यागम मानानि_2; }
		RTLIL::Const संक्रियकः[](addr_t addr) ध्रुव {
			log_assert(addr - आधारः_2 < आकार());
			प्रत्यागम मानानि_2.extract((addr - आधारः_2) * दत्त_विस्तारः_2, दत्त_विस्तारः_2);
		}
		RTLIL::Const at_विस्थापनम्(addr_t विस्थापनम्) ध्रुव { प्रत्यागम (*this)[आधारः_2 + विस्थापनम्]; }
	};
	class iterator {
		MemContents ध्रुव *स्मृतिः_2;
		// storing addr instead of an iterator gives more well-defined behaviour under insertions/deletions
		// use ~0 for end so that all end iterators compare the same
		addr_t _addr;
		मित्रम् class MemContents;
		iterator(MemContents ध्रुव *स्मृतिः_3, addr_t addr) : स्मृतिः_2(स्मृतिः_3), _addr(addr) {}
	सार्वजनिक:
		using iterator_category = std::निवेशः_iterator_चिह्नकम्;
		using मानम्_type = range;
		using pointer = arrow_proxy<range>;
		using reference = range;
		using difference_type = addr_t;
		reference संक्रियकः *() ध्रुव { प्रत्यागम range(स्मृतिः_2->दत्त_विस्तारः_2, _addr, स्मृतिः_2->मानानि_2.at(_addr)); }
		pointer संक्रियकः->() ध्रुव { प्रत्यागम arrow_proxy<range>(**this); }
		bool संक्रियकः !=(iterator ध्रुव &other) ध्रुव { प्रत्यागम स्मृतिः_2 != other.स्मृतिः_2 || _addr != other._addr; }
		bool संक्रियकः ==(iterator ध्रुव &other) ध्रुव { प्रत्यागम !(*this != other); }
		iterator &संक्रियकः++();
	};
	MemContents(पूर्णाङ्क addr_विस्तारः, पूर्णाङ्क दत्त_विस्तारः, RTLIL::Const default_मानम्_2)
		: addr_विस्तारः_2(addr_विस्तारः), दत्त_विस्तारः_2(दत्त_विस्तारः)
		, default_मानम्((default_मानम्_2.extu(दत्त_विस्तारः), std::move(default_मानम्_2)))
	{ log_assert(addr_विस्तारः_2 > 0 && addr_विस्तारः_2 < (पूर्णाङ्क)परिमाण(addr_t) * 8); log_assert(दत्त_विस्तारः_2 > 0); }
	MemContents(पूर्णाङ्क addr_विस्तारः, पूर्णाङ्क दत्त_विस्तारः) : MemContents(addr_विस्तारः, दत्त_विस्तारः, RTLIL::Const(State::Sx, दत्त_विस्तारः)) {}
	explicit MemContents(Mem *mem);
	पूर्णाङ्क addr_विस्तारः() ध्रुव { प्रत्यागम addr_विस्तारः_2; }
	पूर्णाङ्क दत्त_विस्तारः() ध्रुव { प्रत्यागम दत्त_विस्तारः_2; }
	RTLIL::Const ध्रुव &default_मानम्_2() ध्रुव { प्रत्यागम default_मानम्; }
	// return the value at the address if it exists, the default_value of the memory otherwise. address must not exceed 2**addr_width.
	RTLIL::Const संक्रियकः [](addr_t addr) ध्रुव;
	// return the number of defined words in the range [begin_addr, end_addr)
	addr_t गणनम्_range(addr_t begin_addr, addr_t अन्तः_addr) ध्रुव;
	// allocate memory for the range [begin_addr, end_addr), but leave the contents undefined.
	शून्य reserve_range(addr_t begin_addr, addr_t अन्तः_addr) { _reserve_range(begin_addr, अन्तः_addr); }
	// insert multiple words (provided as a single concatenated RTLIL::Const) at the given address, overriding any previous assignment.
	शून्य निवेशय_concatenated(addr_t addr, RTLIL::Const ध्रुव &मानानि);
	// insert multiple words at the given address, overriding any previous assignment.
	प्रतिरूपम्<typename Iterator> शून्य निवेशय_range(addr_t addr, Iterator begin, Iterator अन्तः) {
		स्वचालित शब्दाः_2 = अन्तः - begin;
		log_assert(addr < (addr_t)(1<<addr_विस्तारः_2)); log_assert(शब्दाः_2 <= (addr_t)(1<<addr_विस्तारः_2) - addr);
		स्वचालित range = _reserve_range(addr, addr + शब्दाः_2);
		स्वचालित it = range_दत्त(range, addr);
		प्रति(; begin != अन्तः; ++begin)
			it = _range_write(it, *begin);
	}
	// undefine all words in the range [begin_addr, end_addr)
	शून्य clear_range(addr_t begin_addr, addr_t अन्तः_addr);
	// check invariants, abort if invariants failed
	शून्य check();
	iterator अन्तः() ध्रुव { प्रत्यागम iterator(nullptr, ~(addr_t) 0); }
	iterator begin() ध्रुव { प्रत्यागम मानानि_2.रिक्तानि() ? अन्तः() : iterator(this, मानानि_2.begin()->first); }
	bool रिक्तानि() ध्रुव { प्रत्यागम मानानि_2.रिक्तानि(); }
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
