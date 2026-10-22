#جے_بنا_تعریف بن_ورتیا_بٹ_H
#تعریف_کرو بن_ورتیا_بٹ_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية بن_ورتیا_بٹ
{
	SigMap sigmap;
	pool<RTLIL::Sig_ثنائی_ہندسہ> ورتیا;

	بن_ورتیا_بٹ(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_ورتیا(module, ct);
	}

	بن_ورتیا_بٹ(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_ورتیا(module, ct);
	}

	فراغ collect_ورتیا(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		لكل (خودکار cell : module->cells())
			لكل (خودکار &conn : cell->connections())
				إذا (!ct.cell_output(cell->type, conn.پہلا))
					جوڑو_ورتیا(conn.دوجا);

		لكل (خودکار wire : module->wires())
			إذا (wire->پورٹ_output)
				جوڑو_ورتیا(wire);
	}

	فراغ جوڑو_ورتیا(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار ثنائی_ہندسہ : sigmap(sig))
			إذا (ثنائی_ہندسہ.wire != nullptr)
				ورتیا.داخل_کرو(ثنائی_ہندسہ);
	}

	bool check(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		RTLIL::Sig_ثنائی_ہندسہ mapped = sigmap(ثنائی_ہندسہ);
		أرجع mapped.wire != nullptr && !ورتیا.گنتی(mapped);
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
