#اگر_غیر_تعریف_شدہ غیر_مستعمل_بٹ_H
#تعریف_کرو غیر_مستعمل_بٹ_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ساخت غیر_مستعمل_بٹ
{
	SigMap sigmap;
	pool<RTLIL::Sig_ثنائی_ہندسہ> مستعمل;

	غیر_مستعمل_بٹ(RTLIL::Module *module) : sigmap(module)
	{
		NewCellTypes ct(module->design);
		collect_مستعمل(module, ct);
	}

	غیر_مستعمل_بٹ(RTLIL::Module *module, ثابت NewCellTypes &ct) : sigmap(module)
	{
		collect_مستعمل(module, ct);
	}

	خالی collect_مستعمل(RTLIL::Module *module, ثابت NewCellTypes &ct)
	{
		برائے (خودکار cell : module->cells())
			برائے (خودکار &conn : cell->connections())
				اگر (!ct.cell_output(cell->type, conn.پہلا))
					جوڑو_مستعمل(conn.دوسرا);

		برائے (خودکار wire : module->wires())
			اگر (wire->پورٹ_output)
				جوڑو_مستعمل(wire);
	}

	خالی جوڑو_مستعمل(ثابت RTLIL::SigSpec &sig)
	{
		برائے (خودکار ثنائی_ہندسہ : sigmap(sig))
			اگر (ثنائی_ہندسہ.wire != nullptr)
				مستعمل.داخل_کرو(ثنائی_ہندسہ);
	}

	bool check(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		RTLIL::Sig_ثنائی_ہندسہ mapped = sigmap(ثنائی_ہندسہ);
		واپس mapped.wire != nullptr && !مستعمل.گنتی(mapped);
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
