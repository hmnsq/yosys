#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

কাঠামো UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ধ্রুবক NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	শূন্য collect_used(RTLIL::Module *module, ধ্রুবক NewCellTypes &ct)
	{
		জন্য (auto cell : module->cells())
			জন্য (auto &conn : cell->connections())
				যদি (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		জন্য (auto wire : module->wires())
			যদি (wire->port_output)
				add_used(wire);
	}

	শূন্য add_used(ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (auto bit : sigmap(sig))
			যদি (bit.wire != nullptr)
				used.ঢোকাও(bit);
	}

	bool check(RTLIL::SigBit bit) ধ্রুবক
	{
		RTLIL::SigBit mapped = sigmap(bit);
		ফেরত mapped.wire != nullptr && !used.গুনতি(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
