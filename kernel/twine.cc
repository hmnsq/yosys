#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

سانچا بنية HashConsPool<TwinePool, Twine_گرہ, IdString>;

std::سمتیہ<Twine_گرہ> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	إذا (ready())
		أرجع;
	log_assert(nodes_.خالی_تھاواں());
	nodes_.reserve(STATIC_TWINE_سرا);
	لكل (ثابت حرف *name : ID::static_ناں)
		nodes_.emplace_پٹھ(std::string_view(name));
}

ثابت Twine_گرہ &StaticTwines::گرہ(ناپ_t idx) { أرجع nodes_[idx]; }
منطقي StaticTwines::ready() { أرجع nodes_.ناپ() == STATIC_TWINE_سرا; }

int64_t twine_gc_ns;
صحيح twine_gc_گنتی;

std::string IdString::handle_token() ثابت {
	أرجع stringf("%s@%zu", is_عوامی() ? "$pub" : "$priv", untag().raw());
}

ناپ_t IdString::handle_token_اگیتر(std::string_view token, منطقي &is_عوامی_2) {
	إذا (token.substr(0, 5) == "$pub@") {
		is_عوامی_2 = سچ;
		أرجع 5;
	}
	إذا (token.substr(0, 6) == "$priv@") {
		is_عوامی_2 = خطأ;
		أرجع 6;
	}
	أرجع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_سرا);
	std::string نتیجہ = ref.is_عوامی() ? "\\" : "";
	نتیجہ += static_ناں[idx.raw()];
	أرجع نتیجہ;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_سرا);
	أرجع static_ناں[idx.raw()];
}

TwineSpec::TwineSpec(پتی v) : انکڑے(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : انکڑے(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : انکڑے(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.ناپ() <= MAX_LEN);
	release();
	len_ = content.ناپ();
	إذا (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		حافظہ_نقل(ptr_, content.انکڑے(), len_);
	} وإلا {
		حافظہ_نقل(inl_, content.انکڑے(), len_);
	}
}

فراغ SmallString::release()
{
	إذا (len_ > INLINE_CAP)
		مٹاؤ[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	حافظہ_نقل(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::عامل=(ثابت SmallString &other)
{
	إذا (هذا != &other)
		store(other.view());
	أرجع *هذا;
}

SmallString &SmallString::عامل=(SmallString &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	release();
	حافظہ_نقل(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	أرجع *هذا;
}

SmallString::~SmallString() { release(); }

Twine_گرہ::Twine_گرہ(Twine_گرہ &&other) noexcept
	: متن_2(std::move(other.متن_2)), اگیتر_3(other.اگیتر_3), hash_(other.hash_)
{
	other.اگیتر_3 = DEAD;
}

Twine_گرہ &Twine_گرہ::عامل=(Twine_گرہ &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	متن_2 = std::move(other.متن_2);
	اگیتر_3 = other.اگیتر_3;
	hash_ = other.hash_;
	other.اگیتر_3 = DEAD;
	أرجع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	إذا (خودکار *پتی_2 = std::get_if<پتی>(&انکڑے))
		أرجع پتی_2->s;
	خودکار &autosfx = std::get<AutoSuffix>(انکڑے);
	أرجع *autosfx.اگیتر_2 + autosfx.پوچھ;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	إذا (!name.خالی_تھاواں() && name[0] == '\\')
		أرجع {name.substr(1), سچ};
	أرجع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		إذا ((بنانشانی)ch <= (بنانشانی)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(اگلا_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(اگلا_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(اگلا_serial()) {}

TwinePool& TwinePool::عامل=(ثابت TwinePool& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عامل=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = اگلا_serial();
	أرجع *هذا;
}

TwinePool& TwinePool::عامل=(TwinePool&& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عامل=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = اگلا_serial();
	أرجع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	أرجع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	أرجع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	إذا constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_گرہ& TwinePool::عامل[](IdString ref) ثابت {
	check_owned(ref);
	أرجع HashConsPool::عامل[](ref);
}

ناپ_t TwinePool::gc(pool<IdString>& roots) {
	لكل (خودکار &it : auto_prefixes)
		roots.داخل_کرو(it.دوجا);
	أرجع HashConsPool::gc(roots);
}

ثابت Twine_گرہ& TwinePool::static_گرہ(ناپ_t idx) { أرجع StaticTwines::گرہ(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

ناپ_t TwinePool::اگلا_serial() {
	ساكن ناپ_t counter = 0;
	ناپ_t serial = ++counter;
	إذا constexpr (IdString::MAX_SERIAL == 0)
		أرجع serial;
	وإلا
		أرجع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::جوڑو_inner(Twine_گرہ t) {
	إذا (free_list.خالی_تھاواں() && STATIC_گنتی + backing.ناپ() > IdString::MAX_اشاریہ)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_اشاریہ - STATIC_گنتی);
	أرجع HashConsPool::جوڑو_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_گرہ& twine = (*هذا)[ref];
	بدل (twine.قسم()) {
	حالة Twine_گرہ::قسم_2::Dead:
		os << "Dead()";
		اكسر;
	حالة Twine_گرہ::قسم_2::پتی:
		os << "Leaf(\"" << twine.متن() << "\")";
		اكسر;
	حالة Twine_گرہ::قسم_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.اگیتر_2(), os);
		os << ", tail: \"" << twine.متن() << "\")";
		اكسر;
	}
	إذا (ref.is_عوامی())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& باہرلا) ثابت {
	إذا (ref == IdString::Null)
		أرجع;
	إذا (ref.is_عوامی())
		باہرلا += '\\';
	ثابت Twine_گرہ& twine = (*هذا)[ref];
	بدل (twine.قسم()) {
	حالة Twine_گرہ::قسم_2::Dead:
		اكسر;
	حالة Twine_گرہ::قسم_2::Suffix:
		append_str(twine.اگیتر_2(), باہرلا);
		[[fallthrough]];
	حالة Twine_گرہ::قسم_2::پتی:
		باہرلا += twine.متن();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string باہرلا;
	append_str(ref, باہرلا);
	أرجع باہرلا;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	أرجع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t اگیتر_2, std::string_view متن) ثابت {
	uint32_t hash = اگیتر_2 == Twine_گرہ::NO_اگیتر ? 0 : (*هذا)[IdString(اگیتر_2)].hash();
	أرجع Twine_گرہ::extend_hash(hash, متن);
}

IdString TwinePool::لبھو_content(uint32_t اگیتر_2, std::string_view متن) ثابت {
	أرجع لبھو_content(اگیتر_2, متن, content_hash(اگیتر_2, متن));
}

IdString TwinePool::intern(uint32_t اگیتر_2, std::string_view متن) {
	log_assert(!Multithreading::سرگرم());
	uint32_t hash = content_hash(اگیتر_2, متن);
	IdString ref = لبھو_content(اگیتر_2, متن, hash);
	إذا (ref != IdString::Null)
		أرجع ref;
	أرجع جوڑو_inner(Twine_گرہ{اگیتر_2, متن, hash});
}

IdString TwinePool::لبھو(std::string_view name) ثابت {
	إذا (name.خالی_تھاواں())
		أرجع IdString::Null;
	check_name(name);
	خودکار [content, is_عوامی_2] = unescape_name(name);
	أرجع stamp(لبھو_content(Twine_گرہ::NO_اگیتر, content).ٹیگ(is_عوامی_2));
}

IdString TwinePool::لبھو(TwineSpec t) ثابت {
	إذا (std::holds_alternative<TwineSpec::AutoSuffix>(t.انکڑے))
		أرجع stamp(لبھو_content(Twine_گرہ::NO_اگیتر, t.content_str()));
	إذا (خودکار *پتی_2 = std::get_if<TwineSpec::پتی>(&t.انکڑے))
		أرجع stamp(لبھو_content(Twine_گرہ::NO_اگیتر, پتی_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.انکڑے);
	أرجع stamp(لبھو_content((uint32_t)sfx.اگیتر_2.untag().raw(), sfx.پوچھ).ٹیگ(sfx.اگیتر_2.is_عوامی()));
}

IdString TwinePool::جوڑو(TwineSpec t) {
	إذا (خودکار *ap = std::get_if<TwineSpec::AutoSuffix>(&t.انکڑے))
		أرجع جوڑو(auto_اگیتر(ap->اگیتر_2), ap->پوچھ);
	إذا (خودکار *پتی_2 = std::get_if<TwineSpec::پتی>(&t.انکڑے))
		أرجع stamp(intern(Twine_گرہ::NO_اگیتر, پتی_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.انکڑے);
	أرجع جوڑو(sfx.اگیتر_2, sfx.پوچھ);
}

IdString TwinePool::auto_اگیتر(ثابت std::string *اگیتر_2) {
	خودکار it = auto_prefixes.لبھو(اگیتر_2);
	إذا (it != auto_prefixes.سرا())
		أرجع it->دوجا;
	IdString ref = intern(Twine_گرہ::NO_اگیتر, *اگیتر_2);
	auto_prefixes[اگیتر_2] = ref;
	أرجع ref;
}

IdString TwinePool::جوڑو(IdString اگیتر_2, std::string_view پوچھ) {
	أرجع stamp(intern((uint32_t)اگیتر_2.untag().raw(), پوچھ).ٹیگ(اگیتر_2.is_عوامی()));
}

IdString TwinePool::جوڑو(std::string_view s) {
	إذا (s.خالی_تھاواں())
		أرجع IdString::Null;
	check_name(s);
	خودکار [content, is_عوامی_2] = unescape_name(s);
	أرجع stamp(intern(Twine_گرہ::NO_اگیتر, content).ٹیگ(is_عوامی_2));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString لبھیا = لبھو_from(src, ref);
	إذا (لبھیا != IdString::Null || ref == IdString::Null)
		أرجع لبھیا;
	ثابت Twine_گرہ& t = src[ref.untag()];
	إذا (t.is_dead())
		أرجع IdString::Null;
	uint32_t اگیتر_2 = Twine_گرہ::NO_اگیتر;
	إذا (t.is_suffix())
		اگیتر_2 = (uint32_t)copy_from(src, t.اگیتر_2()).untag().raw();
	أرجع stamp(intern(اگیتر_2, t.متن()).ٹیگ(ref.is_عوامی()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	أرجع "#" + std::منزل_string((uint64_t)serial_) + ":"
		+ std::منزل_string((uint64_t)stamp(ref).بٹ());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	إذا (token.ناپ() < 2 || token[0] != '#')
		أرجع IdString::Null;
	ناپ_t sep = token.لبھو(':');
	إذا (sep == std::string_view::npos)
		أرجع IdString::Null;
	ناپ_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		إذا (parts[i].خالی_تھاواں())
			أرجع IdString::Null;
		لكل (حرف c : parts[i]) {
			إذا (c < '0' || c > '9')
				أرجع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	إذا (fields[0] != serial_)
		أرجع IdString::Null;
	IdString ref(fields[1]);
	إذا (ref == IdString::Null)
		أرجع IdString::Null;
	أرجع is_جیوندا(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr ناپ_t SEGMENT_INLINE_ڈونگھائی = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view پوچھ);
	std::string_view peek();
	فراغ advance(ناپ_t n);
	ناپ_t کل_ناپ() ثابت;
خاص:
	فراغ ڈالو(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_ڈونگھائی];
	std::سمتیہ<std::string_view> spill;
	ناپ_t گنتی = 0;
	ناپ_t pos = 0;
	ناپ_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	إذا (ref == IdString::Null)
		أرجع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت Twine_گرہ &t = pool[cur];
		بدل (t.قسم()) {
		حالة Twine_گرہ::قسم_2::Suffix:
			ڈالو(t.متن());
			cur = t.اگیتر_2();
			استمر;
		حالة Twine_گرہ::قسم_2::پتی:
			ڈالو(t.متن());
			اكسر;
		حالة Twine_گرہ::قسم_2::Dead:
			اكسر;
		}
		اكسر;
	}
	إذا (ref.is_عوامی())
		ڈالو("\\");
	std::الٹ(segs(), segs() + گنتی);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view پوچھ)
	: TwineSegments(pool, سر)
{
	ڈالو(پوچھ);
}

std::string_view TwineSegments::peek()
{
	طالما (pos < گنتی) {
		std::string_view seg = segs()[pos];
		إذا (off < seg.ناپ())
			أرجع seg.substr(off);
		pos++;
		off = 0;
	}
	أرجع {};
}

فراغ TwineSegments::advance(ناپ_t n) { off += n; }

ناپ_t TwineSegments::کل_ناپ() ثابت
{
	ناپ_t کل = 0;
	لكل (ناپ_t i = 0; i < گنتی; i++)
		کل += segs()[i].ناپ();
	أرجع کل;
}

فراغ TwineSegments::ڈالو(std::string_view seg)
{
	إذا (spill.خالی_تھاواں() && گنتی < SEGMENT_INLINE_ڈونگھائی) {
		inline_segs[گنتی++] = seg;
		أرجع;
	}
	إذا (spill.خالی_تھاواں())
		spill.سونپو(inline_segs, inline_segs + گنتی);
	spill.ڈالو_پٹھ(seg);
	گنتی++;
}

std::string_view *TwineSegments::segs() { أرجع spill.خالی_تھاواں() ? inline_segs : spill.انکڑے(); }
ثابت std::string_view *TwineSegments::segs() ثابت { أرجع spill.خالی_تھاواں() ? inline_segs : spill.انکڑے(); }

ناپ_t TwinePool::str_ناپ(IdString ref) ثابت
{
	أرجع TwineSegments(*هذا, ref).کل_ناپ();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view اگیتر_2) ثابت
{
	TwineSegments segs(*هذا, ref);
	طالما (!اگیتر_2.خالی_تھاواں()) {
		std::string_view seg = segs.peek();
		إذا (seg.خالی_تھاواں())
			أرجع خطأ;
		ناپ_t n = std::min(seg.ناپ(), اگیتر_2.ناپ());
		إذا (std::memcmp(seg.انکڑے(), اگیتر_2.انکڑے(), n) != 0)
			أرجع خطأ;
		segs.advance(n);
		اگیتر_2.remove_اگیتر(n);
	}
	أرجع سچ;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	أرجع str_ناپ(ref) == name.ناپ() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	طالما (سچ) {
		std::string_view x = sa.peek(), y = sb.peek();
		إذا (x.خالی_تھاواں() || y.خالی_تھاواں())
			أرجع x.خالی_تھاواں() ? (y.خالی_تھاواں() ? 0 : -1) : 1;
		ناپ_t n = std::min(x.ناپ(), y.ناپ());
		إذا (صحيح diff = std::memcmp(x.انکڑے(), y.انکڑے(), n); diff != 0)
			أرجع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::لبھو_content(uint32_t اگیتر_2, std::string_view متن, uint32_t hash) ثابت {
	أرجع لبھو_hashed(hash, [&](IdString ref) {
		ثابت Twine_گرہ &t = (*هذا)[ref];
		إذا (t == Twine_گرہ::کلید_2{اگیتر_2, متن})
			أرجع سچ;
		IdString سر = اگیتر_2 == Twine_گرہ::NO_اگیتر ? IdString::Null : IdString(اگیتر_2);
		TwineSegments sa(*هذا, ref), sb(*هذا, سر, متن);
		أرجع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::لبھو_from(ثابت TwinePool& src, IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	IdString idx = ref.untag();
	ثابت Twine_گرہ& t = src[idx];
	إذا (t.is_dead())
		أرجع IdString::Null;
	IdString لبھیا = لبھو_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		أرجع compare_segments(sa, sb) == 0;
	});
	أرجع stamp(لبھیا.ٹیگ(ref.is_عوامی()));
}

IdString TwinePool::اگیتر_دا(IdString ref) ثابت {
	ثابت Twine_گرہ &t = (*هذا)[ref];
	إذا (!t.is_suffix())
		أرجع IdString::Null;
	أرجع stamp(t.اگیتر_2().ٹیگ(ref.is_عوامی()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	إذا (a == b)
		أرجع 0;
	إذا (a == IdString::Null)
		أرجع -1;
	إذا (b == IdString::Null)
		أرجع 1;

	إذا (a.is_عوامی() == b.is_عوامی()) {
		ثابت Twine_گرہ &ta = (*هذا)[a.untag()];
		ثابت Twine_گرہ &tb = (*هذا)[b.untag()];
		إذا (ta.is_پتی() && tb.is_پتی())
			أرجع ta.متن().compare(tb.متن());
		إذا (ta.is_suffix() && tb.is_suffix() && ta.اگیتر_2() == tb.اگیتر_2())
			أرجع ta.متن().compare(tb.متن());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	أرجع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_سرا
