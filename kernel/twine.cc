#شامل "kernel/twine.h"
#شامل "kernel/hashcons_impl.h"
#شامل "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

سانچہ ساخت HashConsPool<TwinePool, Twine_گرہ, IdString>;

std::سمتیہ<Twine_گرہ> StaticTwines::nodes_;

خالی StaticTwines::init() {
	اگر (ready())
		واپس;
	log_assert(nodes_.خالی_خانے());
	nodes_.reserve(STATIC_TWINE_انتہا);
	برائے (ثابت حرف *name : ID::static_نام)
		nodes_.emplace_عقب(std::string_view(name));
}

ثابت Twine_گرہ &StaticTwines::گرہ(سائز_t idx) { واپس nodes_[idx]; }
منطقی StaticTwines::ready() { واپس nodes_.سائز() == STATIC_TWINE_انتہا; }

int64_t twine_gc_ns;
صحیح twine_gc_گنتی;

std::string IdString::handle_token() ثابت {
	واپس stringf("%s@%zu", is_عوامی() ? "$pub" : "$priv", untag().raw());
}

سائز_t IdString::handle_token_سابقہ(std::string_view token, منطقی &is_عوامی_2) {
	اگر (token.substr(0, 5) == "$pub@") {
		is_عوامی_2 = سچ;
		واپس 5;
	}
	اگر (token.substr(0, 6) == "$priv@") {
		is_عوامی_2 = جھوٹ;
		واپس 6;
	}
	واپس 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_انتہا);
	std::string نتیجہ = ref.is_عوامی() ? "\\" : "";
	نتیجہ += static_نام[idx.raw()];
	واپس نتیجہ;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_انتہا);
	واپس static_نام[idx.raw()];
}

TwineSpec::TwineSpec(برگ v) : ڈیٹا(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : ڈیٹا(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : ڈیٹا(std::move(v)) {}

خالی SmallString::store(std::string_view content)
{
	log_assert(content.سائز() <= MAX_LEN);
	release();
	len_ = content.سائز();
	اگر (len_ > INLINE_CAP) {
		ptr_ = نیا حرف[len_];
		حافظہ_نقل(ptr_, content.ڈیٹا(), len_);
	} ورنہ {
		حافظہ_نقل(inl_, content.ڈیٹا(), len_);
	}
}

خالی SmallString::release()
{
	اگر (len_ > INLINE_CAP)
		حذف[] ptr_;
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
	اگر (یہ != &other)
		store(other.view());
	واپس *یہ;
}

SmallString &SmallString::عامل=(SmallString &&other) noexcept
{
	اگر (یہ == &other)
		واپس *یہ;
	release();
	حافظہ_نقل(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	واپس *یہ;
}

SmallString::~SmallString() { release(); }

Twine_گرہ::Twine_گرہ(Twine_گرہ &&other) noexcept
	: متن_2(std::move(other.متن_2)), سابقہ_3(other.سابقہ_3), hash_(other.hash_)
{
	other.سابقہ_3 = DEAD;
}

Twine_گرہ &Twine_گرہ::عامل=(Twine_گرہ &&other) noexcept
{
	اگر (یہ == &other)
		واپس *یہ;
	متن_2 = std::move(other.متن_2);
	سابقہ_3 = other.سابقہ_3;
	hash_ = other.hash_;
	other.سابقہ_3 = DEAD;
	واپس *یہ;
}

std::string TwineSpec::content_str() ثابت {
	اگر (خودکار *برگ_2 = std::get_if<برگ>(&ڈیٹا))
		واپس برگ_2->s;
	خودکار &autosfx = std::get<AutoSuffix>(ڈیٹا);
	واپس *autosfx.سابقہ_2 + autosfx.دم;
}

ساکن std::pair<std::string_view, منطقی> unescape_name(std::string_view name) {
	اگر (!name.خالی_خانے() && name[0] == '\\')
		واپس {name.substr(1), سچ};
	واپس {name, جھوٹ};
}

ساکن خالی check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	برائے (حرف ch : name)
		اگر ((غیرعلامتی)ch <= (غیرعلامتی)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(اگلا_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(اگلا_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(اگلا_serial()) {}

TwinePool& TwinePool::عامل=(ثابت TwinePool& other) {
	اگر (یہ == &other)
		واپس *یہ;
	HashConsPool::عامل=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = اگلا_serial();
	واپس *یہ;
}

TwinePool& TwinePool::عامل=(TwinePool&& other) {
	اگر (یہ == &other)
		واپس *یہ;
	HashConsPool::عامل=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = اگلا_serial();
	واپس *یہ;
}

منطقی TwinePool::owns(IdString ref) ثابت {
	واپس ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	اگر (ref == IdString::Null || ID::is_static(ref))
		واپس ref;
	واپس ref.stamped(serial_);
}

خالی TwinePool::check_owned(IdString ref) ثابت {
	اگر constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_گرہ& TwinePool::عامل[](IdString ref) ثابت {
	check_owned(ref);
	واپس HashConsPool::عامل[](ref);
}

سائز_t TwinePool::gc(pool<IdString>& roots) {
	برائے (خودکار &it : auto_prefixes)
		roots.داخل_کرو(it.دوسرا);
	واپس HashConsPool::gc(roots);
}

ثابت Twine_گرہ& TwinePool::static_گرہ(سائز_t idx) { واپس StaticTwines::گرہ(idx); }
خالی TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

سائز_t TwinePool::اگلا_serial() {
	ساکن سائز_t شمار_2 = 0;
	سائز_t serial = ++شمار_2;
	اگر constexpr (IdString::MAX_SERIAL == 0)
		واپس serial;
	ورنہ
		واپس serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::جوڑو_inner(Twine_گرہ t) {
	اگر (free_list.خالی_خانے() && STATIC_گنتی + backing.سائز() > IdString::MAX_اشاریہ)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_اشاریہ - STATIC_گنتی);
	واپس HashConsPool::جوڑو_inner(std::move(t));
}

خالی TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_گرہ& twine = (*یہ)[ref];
	انتخاب (twine.قسم()) {
	حالت Twine_گرہ::قسم_2::Dead:
		os << "Dead()";
		توڑو;
	حالت Twine_گرہ::قسم_2::برگ:
		os << "Leaf(\"" << twine.متن() << "\")";
		توڑو;
	حالت Twine_گرہ::قسم_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.سابقہ_2(), os);
		os << ", tail: \"" << twine.متن() << "\")";
		توڑو;
	}
	اگر (ref.is_عوامی())
		os << " pub";
}

خالی TwinePool::append_str(IdString ref, std::string& برآمد) ثابت {
	اگر (ref == IdString::Null)
		واپس;
	اگر (ref.is_عوامی())
		برآمد += '\\';
	ثابت Twine_گرہ& twine = (*یہ)[ref];
	انتخاب (twine.قسم()) {
	حالت Twine_گرہ::قسم_2::Dead:
		توڑو;
	حالت Twine_گرہ::قسم_2::Suffix:
		append_str(twine.سابقہ_2(), برآمد);
		[[fallthrough]];
	حالت Twine_گرہ::قسم_2::برگ:
		برآمد += twine.متن();
		توڑو;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string برآمد;
	append_str(ref, برآمد);
	واپس برآمد;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	واپس str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t سابقہ_2, std::string_view متن) ثابت {
	uint32_t hash = سابقہ_2 == Twine_گرہ::NO_سابقہ ? 0 : (*یہ)[IdString(سابقہ_2)].hash();
	واپس Twine_گرہ::extend_hash(hash, متن);
}

IdString TwinePool::ڈھونڈو_content(uint32_t سابقہ_2, std::string_view متن) ثابت {
	واپس ڈھونڈو_content(سابقہ_2, متن, content_hash(سابقہ_2, متن));
}

IdString TwinePool::intern(uint32_t سابقہ_2, std::string_view متن) {
	log_assert(!Multithreading::فعال());
	uint32_t hash = content_hash(سابقہ_2, متن);
	IdString ref = ڈھونڈو_content(سابقہ_2, متن, hash);
	اگر (ref != IdString::Null)
		واپس ref;
	واپس جوڑو_inner(Twine_گرہ{سابقہ_2, متن, hash});
}

IdString TwinePool::ڈھونڈو(std::string_view name) ثابت {
	اگر (name.خالی_خانے())
		واپس IdString::Null;
	check_name(name);
	خودکار [content, is_عوامی_2] = unescape_name(name);
	واپس stamp(ڈھونڈو_content(Twine_گرہ::NO_سابقہ, content).لیبل(is_عوامی_2));
}

IdString TwinePool::ڈھونڈو(TwineSpec t) ثابت {
	اگر (std::holds_alternative<TwineSpec::AutoSuffix>(t.ڈیٹا))
		واپس stamp(ڈھونڈو_content(Twine_گرہ::NO_سابقہ, t.content_str()));
	اگر (خودکار *برگ_2 = std::get_if<TwineSpec::برگ>(&t.ڈیٹا))
		واپس stamp(ڈھونڈو_content(Twine_گرہ::NO_سابقہ, برگ_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ڈیٹا);
	واپس stamp(ڈھونڈو_content((uint32_t)sfx.سابقہ_2.untag().raw(), sfx.دم).لیبل(sfx.سابقہ_2.is_عوامی()));
}

IdString TwinePool::جوڑو(TwineSpec t) {
	اگر (خودکار *ap = std::get_if<TwineSpec::AutoSuffix>(&t.ڈیٹا))
		واپس جوڑو(auto_سابقہ(ap->سابقہ_2), ap->دم);
	اگر (خودکار *برگ_2 = std::get_if<TwineSpec::برگ>(&t.ڈیٹا))
		واپس stamp(intern(Twine_گرہ::NO_سابقہ, برگ_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.ڈیٹا);
	واپس جوڑو(sfx.سابقہ_2, sfx.دم);
}

IdString TwinePool::auto_سابقہ(ثابت std::string *سابقہ_2) {
	خودکار it = auto_prefixes.ڈھونڈو(سابقہ_2);
	اگر (it != auto_prefixes.انتہا())
		واپس it->دوسرا;
	IdString ref = intern(Twine_گرہ::NO_سابقہ, *سابقہ_2);
	auto_prefixes[سابقہ_2] = ref;
	واپس ref;
}

IdString TwinePool::جوڑو(IdString سابقہ_2, std::string_view دم) {
	واپس stamp(intern((uint32_t)سابقہ_2.untag().raw(), دم).لیبل(سابقہ_2.is_عوامی()));
}

IdString TwinePool::جوڑو(std::string_view s) {
	اگر (s.خالی_خانے())
		واپس IdString::Null;
	check_name(s);
	خودکار [content, is_عوامی_2] = unescape_name(s);
	واپس stamp(intern(Twine_گرہ::NO_سابقہ, content).لیبل(is_عوامی_2));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString ملا = ڈھونڈو_from(src, ref);
	اگر (ملا != IdString::Null || ref == IdString::Null)
		واپس ملا;
	ثابت Twine_گرہ& t = src[ref.untag()];
	اگر (t.is_dead())
		واپس IdString::Null;
	uint32_t سابقہ_2 = Twine_گرہ::NO_سابقہ;
	اگر (t.is_suffix())
		سابقہ_2 = (uint32_t)copy_from(src, t.سابقہ_2()).untag().raw();
	واپس stamp(intern(سابقہ_2, t.متن()).لیبل(ref.is_عوامی()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	واپس "#" + std::مقصد_string((uint64_t)serial_) + ":"
		+ std::مقصد_string((uint64_t)stamp(ref).بٹ());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	اگر (token.سائز() < 2 || token[0] != '#')
		واپس IdString::Null;
	سائز_t sep = token.ڈھونڈو(':');
	اگر (sep == std::string_view::npos)
		واپس IdString::Null;
	سائز_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	برائے (صحیح i = 0; i < 2; i++) {
		اگر (parts[i].خالی_خانے())
			واپس IdString::Null;
		برائے (حرف c : parts[i]) {
			اگر (c < '0' || c > '9')
				واپس IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	اگر (fields[0] != serial_)
		واپس IdString::Null;
	IdString ref(fields[1]);
	اگر (ref == IdString::Null)
		واپس IdString::Null;
	واپس is_زندہ(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
ساخت TwineSegments {
	ساکن constexpr سائز_t SEGMENT_INLINE_گہرائی = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view دم);
	std::string_view peek();
	خالی advance(سائز_t n);
	سائز_t کل_سائز() ثابت;
نجی:
	خالی ڈالو(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_گہرائی];
	std::سمتیہ<std::string_view> spill;
	سائز_t گنتی = 0;
	سائز_t pos = 0;
	سائز_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	اگر (ref == IdString::Null)
		واپس;
	برائے (IdString cur = ref.untag(); ;) {
		ثابت Twine_گرہ &t = pool[cur];
		انتخاب (t.قسم()) {
		حالت Twine_گرہ::قسم_2::Suffix:
			ڈالو(t.متن());
			cur = t.سابقہ_2();
			جاری;
		حالت Twine_گرہ::قسم_2::برگ:
			ڈالو(t.متن());
			توڑو;
		حالت Twine_گرہ::قسم_2::Dead:
			توڑو;
		}
		توڑو;
	}
	اگر (ref.is_عوامی())
		ڈالو("\\");
	std::الٹ(segs(), segs() + گنتی);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view دم)
	: TwineSegments(pool, سر)
{
	ڈالو(دم);
}

std::string_view TwineSegments::peek()
{
	جب (pos < گنتی) {
		std::string_view seg = segs()[pos];
		اگر (off < seg.سائز())
			واپس seg.substr(off);
		pos++;
		off = 0;
	}
	واپس {};
}

خالی TwineSegments::advance(سائز_t n) { off += n; }

سائز_t TwineSegments::کل_سائز() ثابت
{
	سائز_t کل = 0;
	برائے (سائز_t i = 0; i < گنتی; i++)
		کل += segs()[i].سائز();
	واپس کل;
}

خالی TwineSegments::ڈالو(std::string_view seg)
{
	اگر (spill.خالی_خانے() && گنتی < SEGMENT_INLINE_گہرائی) {
		inline_segs[گنتی++] = seg;
		واپس;
	}
	اگر (spill.خالی_خانے())
		spill.تفویض(inline_segs, inline_segs + گنتی);
	spill.ڈالو_عقب(seg);
	گنتی++;
}

std::string_view *TwineSegments::segs() { واپس spill.خالی_خانے() ? inline_segs : spill.ڈیٹا(); }
ثابت std::string_view *TwineSegments::segs() ثابت { واپس spill.خالی_خانے() ? inline_segs : spill.ڈیٹا(); }

سائز_t TwinePool::str_سائز(IdString ref) ثابت
{
	واپس TwineSegments(*یہ, ref).کل_سائز();
}

منطقی TwinePool::begins_with(IdString ref, std::string_view سابقہ_2) ثابت
{
	TwineSegments segs(*یہ, ref);
	جب (!سابقہ_2.خالی_خانے()) {
		std::string_view seg = segs.peek();
		اگر (seg.خالی_خانے())
			واپس جھوٹ;
		سائز_t n = std::min(seg.سائز(), سابقہ_2.سائز());
		اگر (std::memcmp(seg.ڈیٹا(), سابقہ_2.ڈیٹا(), n) != 0)
			واپس جھوٹ;
		segs.advance(n);
		سابقہ_2.remove_سابقہ(n);
	}
	واپس سچ;
}

منطقی TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	واپس str_سائز(ref) == name.سائز() && begins_with(ref, name);
}

ساکن صحیح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	جب (سچ) {
		std::string_view x = sa.peek(), y = sb.peek();
		اگر (x.خالی_خانے() || y.خالی_خانے())
			واپس x.خالی_خانے() ? (y.خالی_خانے() ? 0 : -1) : 1;
		سائز_t n = std::min(x.سائز(), y.سائز());
		اگر (صحیح diff = std::memcmp(x.ڈیٹا(), y.ڈیٹا(), n); diff != 0)
			واپس diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::ڈھونڈو_content(uint32_t سابقہ_2, std::string_view متن, uint32_t hash) ثابت {
	واپس ڈھونڈو_hashed(hash, [&](IdString ref) {
		ثابت Twine_گرہ &t = (*یہ)[ref];
		اگر (t == Twine_گرہ::کلید_2{سابقہ_2, متن})
			واپس سچ;
		IdString سر = سابقہ_2 == Twine_گرہ::NO_سابقہ ? IdString::Null : IdString(سابقہ_2);
		TwineSegments sa(*یہ, ref), sb(*یہ, سر, متن);
		واپس compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::ڈھونڈو_from(ثابت TwinePool& src, IdString ref) ثابت {
	اگر (ref == IdString::Null || ID::is_static(ref))
		واپس ref;
	IdString idx = ref.untag();
	ثابت Twine_گرہ& t = src[idx];
	اگر (t.is_dead())
		واپس IdString::Null;
	IdString ملا = ڈھونڈو_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*یہ, cand), sb(src, idx);
		واپس compare_segments(sa, sb) == 0;
	});
	واپس stamp(ملا.لیبل(ref.is_عوامی()));
}

IdString TwinePool::سابقہ_کا(IdString ref) ثابت {
	ثابت Twine_گرہ &t = (*یہ)[ref];
	اگر (!t.is_suffix())
		واپس IdString::Null;
	واپس stamp(t.سابقہ_2().لیبل(ref.is_عوامی()));
}

صحیح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	اگر (a == b)
		واپس 0;
	اگر (a == IdString::Null)
		واپس -1;
	اگر (b == IdString::Null)
		واپس 1;

	اگر (a.is_عوامی() == b.is_عوامی()) {
		ثابت Twine_گرہ &ta = (*یہ)[a.untag()];
		ثابت Twine_گرہ &tb = (*یہ)[b.untag()];
		اگر (ta.is_برگ() && tb.is_برگ())
			واپس ta.متن().compare(tb.متن());
		اگر (ta.is_suffix() && tb.is_suffix() && ta.سابقہ_2() == tb.سابقہ_2())
			واپس ta.متن().compare(tb.متن());
	}

	TwineSegments sa(*یہ, a), sb(*یہ, b);
	واپس compare_segments(sa, sb);
}

YOSYS_NAMESPACE_انتہا
