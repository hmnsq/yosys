#ifndef UNUSED_BITS_H
#define UNUSED_BITS_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ساختار UnusedBits
{
	SigMap sigmap;
	pool<RTLIL::SigBit> used;

	UnusedBits(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_used(module, ct);
	}

	UnusedBits(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_used(module, ct);
	}

	تهی collect_used(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		برای (auto cell : module->cells())
			برای (auto &conn : cell->connections())
				اگر (!ct.cell_output(cell->type, conn.first))
					add_used(conn.second);

		برای (auto wire : module->wires())
			اگر (wire->port_output)
				add_used(wire);
	}

	تهی add_used(ثابت RTLIL::SigSpec &sig)
	{
		برای (auto bit : sigmap(sig))
			اگر (bit.wire != nullptr)
				used.درج(bit);
	}

	bool check(RTLIL::SigBit bit) ثابت
	{
		RTLIL::SigBit mapped = sigmap(bit);
		بازگشت mapped.wire != nullptr && !used.شمار(mapped);
	}
};

YOSYS_NAMESPACE_END

#endif
